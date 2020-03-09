/*
 * Main entrypoint for LBF.
 *
 * (c) Kiëd Llaentenn and contributors
 * See the LICENSE.md file for more information.
 */

#include "args.h"
#include "bool.h"
#include "lbf.h"

int
main(int argc, char **argv)
{
	if (argc < 2) return 0;

	--argc;
	++argv;

	opts = (struct Options*) calloc(1, sizeof(struct Options));

	/* parse arguments */
	ARGBEGIN {
	case 'v':
		opts->verbose = TRUE;
		break;
	case 'b':
		char *backend = ARGF();

		switch (*backend) {
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
		char *option = ARGF();

		if (!strcmp(option, "enable-dbg"))
			opts->fopt_enable_dbg_command = TRUE;
		else if (!strcmp(option, "enable-lbf-std"))
			opts->fopt_enable_lbf_std = TRUE;
		else if (!strcmp(option, "optimize-multiple-commands"))
			opts->fopt_enable_command_squashing = TRUE;
		else if (!strcmp(option, "optimize-scan-commands"))
			opts->fopt_enable_scan_command = TRUE;
		else if (!strcmp(option, "optimize-nullify-loops"))
			opts->fopt_enable_nullify_command = TRUE;
		else
			die("lbf: error: invalid argument to -f.");
		break;
	case 'W':
		char *warning = ARGF();

		if (!strcmp(warning, "error"))
			opts->wopt_error = TRUE;
		else if (!strcmp(warning, "comments"))
			opts->wopt_comments = TRUE;
		else if (!strcmp(warning, "long-lines"))
			opts->wopt_long_lines = TRUE;
		else if (!strcmp(warning, "dead-code"))
			opts->wopt_dead_code = TRUE;
		else if (!strcmp(warning, "ignored-dbg"))
			opts->wopt_ignore_dbg = TRUE;
		else if (!strcmp(warning, "all"))
			opts->wopt_all = TRUE;
		else
			die("lbf: error: invalid argument to -W.");
		break;
	} ARGEND

	return lbfi_main(stdin);
}
