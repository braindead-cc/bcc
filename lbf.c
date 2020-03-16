/*
 * Main entrypoint for LBF.
 *
 * (c) Kiëd Llaentenn and contributors
 * See the LICENSE.md file for more information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bool.h"
#include "instructions.h"
#include "lbf.h"
#include "lbfc.h"
#include "lbfi.h"
#include "prepare.h"
#include "util.h"

int
main(int argc, char **argv)
{
	if (argc < 2) die("lbf: error: nothing to do, exiting.");

	struct Options *opts = malloc(1 * sizeof(struct Options));

	/* set default options */
	opts->debug                         = FALSE;
	opts->verbose                       = FALSE;
	opts->backend                       = BOPT_C;
	opts->fopt_enable_dbg_command       = FALSE;
	opts->fopt_enable_lbf_std           = FALSE;
	opts->fopt_enable_nullify_command   = FALSE;
	opts->fopt_enable_scan_command      = FALSE;
	opts->fopt_enable_command_squashing = FALSE;
	opts->wopt_error                    = FALSE;
	opts->wopt_comments                 = FALSE;
	opts->wopt_long_lines               = FALSE;
	opts->wopt_dead_code                = FALSE;
	opts->wopt_ignore_dbg               = FALSE;
	opts->wopt_tape_overflow            = FALSE;
	opts->wopt_all                      = FALSE;
	opts->fopt_cell_size                = 8;
	opts->fopt_cell_signed              = FALSE;
	opts->fopt_debug_context            = 32;
	opts->fopt_initial_tape_size        = 256;


	/* parse arguments */
	isize opt = 0;
	while ((opt = getopt(argc, argv, "Vhdvb:f:W:")) != -1) {
		switch (opt) {
		case 'V':
			printf("lbf v%s (build %s)\n", VERSION, BUILDDATE);
			return 0;
			break;
		case 'h':
			printf("usage: lbf [MODE] [ARGS] < source.bf\n");
			printf("modes: lbfi, lbfc\n");
			return 0;
			break;
		case 'd':
			opts->debug = TRUE;
			break;
		case 'v':
			opts->verbose = TRUE;
			break;
		case 'b':
			switch (optarg[0]) {
			case 'C':
			case 'c':
				opts->backend = BOPT_C;
				break;
			case 'A':
			case 'a':
				opts->backend = BOPT_ARM;
				break;
			case 'L':
			case 'l':
				opts->backend = BOPT_LLVM;
				break;
			case 'R':
			case 'r':
				opts->backend = BOPT_RUST;
				break;
			case 'V':
			case 'v':
				opts->backend = BOPT_V;
				break;
			case 'G':
			case 'g':
				opts->backend = BOPT_GO;
				break;
			case 'Q':
			case 'q':
				opts->backend = BOPT_QBE;
				break;
			}
			break;
		case 'f':
			if (!strcmp(optarg, "enable-dbg")) {
				opts->fopt_enable_dbg_command = TRUE;
			} else if (!strcmp(optarg, "enable-lbf-std")) {
				opts->fopt_enable_lbf_std = TRUE;
			} else if (!strcmp(optarg, "optimize-multiple-commands")) {
				opts->fopt_enable_command_squashing = TRUE;
			} else if (!strcmp(optarg, "optimize-scan-commands")) {
				opts->fopt_enable_scan_command = TRUE;
			} else if (!strcmp(optarg, "optimize-nullify-loops")) {
				opts->fopt_enable_nullify_command = TRUE;
			} else if (!strncmp(optarg, "cell-size=", 10)) {
				while (*optarg != '=') ++optarg;
				opts->fopt_cell_size = (u8) strtol(optarg, NULL, 10);
			} else if (!strcmp(optarg, "cell-signed")) {
				opts->fopt_cell_signed = TRUE;
			} else if (!strncmp(optarg, "debug-context=", 14)) {
				while (*optarg != '=') ++optarg;
				opts->fopt_debug_context = (u64) strtol(optarg, NULL, 10);
			} else if (!strncmp(optarg, "initial-tape-size=", 18)) {
				while (*optarg != '=') ++optarg;
				opts->fopt_initial_tape_size = (u64) strtol(optarg, NULL, 10);
			} else {
				die("lbf: error: '%s': invalid argument to -f.", optarg);
			}
			break;
		case 'W':
			if (!strcmp(optarg, "error"))
				opts->wopt_error = TRUE;
			else if (!strcmp(optarg, "comments"))
				opts->wopt_comments = TRUE;
			else if (!strcmp(optarg, "long-lines"))
				opts->wopt_long_lines = TRUE;
			else if (!strcmp(optarg, "dead-code"))
				opts->wopt_dead_code = TRUE;
			else if (!strcmp(optarg, "ignored-dbg"))
				opts->wopt_ignore_dbg = TRUE;
			else if (!strcmp(optarg, "tape-overflow"))
				opts->wopt_tape_overflow = TRUE;
			else if (!strcmp(optarg, "all"))
				opts->wopt_all = TRUE;
			else
				die("lbf: error: '%s': invalid argument to -W.", optarg);
			break;
		default:
			die("lbf: error: invalid argument.");
			break;
		}
	}

	/* print options */
	if (opts->debug) {
		debug("debug = %i", opts->debug);
		debug("verbose = %i", opts->verbose);
		/* TODO: print backend */
		debug("fopt_enable_dbg_command = %i",
				opts->fopt_enable_dbg_command);
		debug("fopt_enable_lbf_std = %i",
				opts->fopt_enable_lbf_std);
		debug("fopt_enable_nullify_command = %i",
				opts->fopt_enable_nullify_command);
		debug("fopt_enable_scan_command = %i",
				opts->fopt_enable_scan_command);
		debug("fopt_enable_command_squashing = %i",
				opts->fopt_enable_command_squashing);
		debug("wopt_error = %i", opts->wopt_error);
		debug("wopt_comments = %i", opts->wopt_comments);
		debug("wopt_long_lines = %i", opts->wopt_long_lines);
		debug("wopt_dead_code = %i", opts->wopt_dead_code);
		debug("wopt_ignore_dbg = %i", opts->wopt_ignore_dbg);
		debug("wopt_tape_overflow = %i", opts->wopt_tape_overflow);
		debug("wopt_all = %i", opts->wopt_all);
		debug("fopt_cell_size = %i", opts->fopt_cell_size);
		debug("fopt_cell_signed = %i", opts->fopt_cell_signed);
		debug("fopt_debug_context = %lli", opts->fopt_debug_context);
		debug("fopt_initial_tape_size = %lli", opts->fopt_initial_tape_size);
	}

	struct Instruction *head = malloc(sizeof(struct Instruction));
	if (head == NULL)
		die("lbf: error: cannot allocate memory:");
	prepare(opts, head);
	if (!strcmp(argv[1], "lbfi"))
		lbfi_main(opts, head);
	else if (!strcmp(argv[1], "lbfc"))
		lbfc_main(opts, head);
}
