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
#include "lbfd.h"
#include "prepare.h"
#include "util.h"

int
main(int argc, char **argv)
{
	if (argc < 2) die("lbf: error: nothing to do, exiting.");

	struct Options *opts = malloc(1 * sizeof(struct Options));

	/* set default options */
	opts->path                          = "-";
	opts->debug                         = FALSE;
	opts->verbose                       = FALSE;
	opts->backend                       = BOPT_C;
	opts->fopt_enable_dbg_command       = FALSE;
	opts->fopt_enable_lbf_std           = FALSE;
	opts->fopt_enable_nullify_command   = FALSE;
	opts->fopt_enable_scan_command      = FALSE;
	opts->fopt_enable_command_squashing = FALSE;
	opts->wopt_long_lines               = FALSE;
	opts->wopt_dead_code                = FALSE;
	opts->wopt_ignore_dbg               = FALSE;
	opts->fopt_cell_size                = 8;
	opts->fopt_cell_signed              = FALSE;
	opts->fopt_initial_tape_size        = 16;
	opts->fopt_comment_char             = ';';
	opts->fopt_eof_char                 = EOF_ZERO;


	/* parse arguments */
	++argv, --argc;
	isize opt = 0;
	while ((opt = getopt(argc, argv, "Vhdvb:f:O:W:")) != -1) {
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
			} else if (!strcmp(optarg, "optimize-scan-loops")) {
				opts->fopt_enable_scan_command = TRUE;
			} else if (!strcmp(optarg, "optimize-nullify-loops")) {
				opts->fopt_enable_nullify_command = TRUE;
			} else if (!strncmp(optarg, "cell-size=", 10)) {
				while (*optarg != '=') ++optarg;
				++optarg;
				opts->fopt_cell_size = (u8) strtol(optarg, NULL, 10);
			} else if (!strcmp(optarg, "cell-signed")) {
				opts->fopt_cell_signed = TRUE;
			} else if (!strncmp(optarg, "initial-tape-size=", 18)) {
				while (*optarg != '=') ++optarg;
				++optarg;
				opts->fopt_initial_tape_size = (u64) strtol(optarg, NULL, 10);
			} else if (!strcmp(optarg, "disable-dynamic-alloc")) {
				opts->fopt_disable_dynamic_alloc = TRUE;
			} else if (!strncmp(optarg, "comment-char=", 13)) {
				while (*optarg != '=') ++optarg;
				++optarg;
				opts->fopt_comment_char = optarg[0];
			} else if (!strncmp(optarg, "eof-value=", 10)) {
				while (*optarg != '=') ++optarg;
				++optarg;

				if (!strcmp(optarg, "0")) {
					opts->fopt_eof_char = EOF_ZERO;
				} else if (!strcmp(optarg, "-1")) {
					opts->fopt_eof_char = EOF_MINUS_1;
				} else if (!strcmp(optarg, "none")) {
					opts->fopt_eof_char = EOF_NO_CHANGE;
				} else {
					die("lbf: error: '%s': invalid"
						"argument for -feof-value",
						optarg);
				}
			} else {
				die("lbf: error: '%s': invalid argument to -f.",
					optarg);
			}
			break;
		case 'O':
			switch (optarg[0]) {
			case '2':
				opts->fopt_enable_nullify_command = TRUE;
				opts->fopt_enable_scan_command = TRUE;
				/* fallthrough */
			case '1':
				opts->fopt_enable_command_squashing = TRUE;
				/* fallthrough */
			case '0':
				break;
			default:
				die("lbf: error: '%c': invalid optimization level.",
						optarg[0]);
			}
			break;
		case 'W':
			if (!strcmp(optarg, "long-lines"))
				opts->wopt_long_lines = TRUE;
			else if (!strcmp(optarg, "dead-code"))
				opts->wopt_dead_code = TRUE;
			else if (!strcmp(optarg, "ignored-dbg"))
				opts->wopt_ignore_dbg = TRUE;
			else
				die("lbf: error: '%s': invalid argument to -W.", optarg);
			break;
		default:
			die("lbf: error: invalid argument.");
			break;
		}
	}

	if (argv[optind] != NULL) opts->path = argv[optind];

	struct Instruction *head = malloc(sizeof(struct Instruction));
	if (head == NULL)
		die("lbf: error: cannot allocate memory:");
	prepare(opts, head);

	if (!strcmp(argv[0], "lbfi"))
		lbfi_main(opts, head);
	else if (!strcmp(argv[0], "lbfc"))
		lbfc_main(opts, head);
	else if (!strcmp(argv[0], "lbfd"))
		lbfd_main(opts, head);
}
