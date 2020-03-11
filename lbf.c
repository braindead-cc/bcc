/*
 * Main entrypoint for LBF.
 *
 * (c) Kiëd Llaentenn and contributors
 * See the LICENSE.md file for more information.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bool.h"
#include "lbf.h"
#include "lbfi.h"
#include "util.h"

int
main(int argc, char **argv)
{
	if (argc < 2) return 0;

	--argc;
	++argv;

	char *path = NULL;           /* input file */
	struct Options *opts = calloc(1, sizeof(struct Options));

	/* set default options */
	opts->verbose                       = FALSE;
	opts->bparams                       = BOPT_C;
	opts->fopt_enable_dbg_command       = FALSE;
	opts->fopt_enable_nullify_command   = FALSE;
	opts->fopt_enable_scan_command      = FALSE;
	opts->fopt_enable_command_squashing = FALSE;
	opts->wopt_error                    = FALSE;
	opts->wopt_comments                 = FALSE;
	opts->wopt_long_lines               = FALSE;
	opts->wopt_dead_code                = FALSE;
	opts->wopt_ignore_dbg               = FALSE;
	opts->wopt_all                      = FALSE;
	opts->fopt_cell_size                = 8;
	opts->fopt_cell_signed              = FALSE;
	opts->fopt_cell_wrap                = TRUE;


	/* parse arguments */
	isize opt = 0;
	while ((opt = getopt(argc, argv, "vb:f:W:i:")) != -1) {
		switch (opt) {
		case 'v':
			opts->verbose = TRUE;
			break;
		case 'b':
			switch (optarg[0]) {
			case 'C':
			case 'c':
				opts->backend = BOPT_C;
				break;
			case 'S':
			case 's':
				opts->backend = BOPT_SHELL;
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
			if (!strcmp(optarg, "enable-dbg"))
				opts->fopt_enable_dbg_command = TRUE;
			else if (!strcmp(optarg, "enable-lbf-std"))
				opts->fopt_enable_lbf_std = TRUE;
			else if (!strcmp(optarg, "optimize-multiple-commands"))
				opts->fopt_enable_command_squashing = TRUE;
			else if (!strcmp(optarg, "optimize-scan-commands"))
				opts->fopt_enable_scan_command = TRUE;
			else if (!strcmp(optarg, "optimize-nullify-loops"))
				opts->fopt_enable_nullify_command = TRUE;
			else
				die("lbf: error: '%s': invalid argument to -f.", optarg);
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
			else if (!strcmp(optarg, "all"))
				opts->wopt_all = TRUE;
			else
				die("lbf: error: '%s': invalid argument to -W.", optarg);
			break;
		/* TODO: make it possible to specify files without -i option */
		case 'i':
			path = optarg;
			break;
		default:
			die("lbf: error: invalid argument.");
			break;
		}
	}

	if (path == NULL) die("lbf: error: nothing to do, exiting.");
	return lbfi_main(path, opts);
}
