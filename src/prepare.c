/*
 * read, lex, parse, optimize, and lint
 * branfsck code.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "instructions.h"
#include "options.h"
#include "opt-nloops.h"
#include "opt-squash.h"
#include "opt-sloops.h"
#include "parser.h"
#include "prepare.h"
#include "status.h"
#include "util.h"

usize
prepare(struct Options *opts, struct Instruction *head)
{
	usize sz = 512;
	char *program_data = malloc(sz * sizeof(char));
	if (program_data == NULL)
		die("lbf: error: cannot read brainfsck code:");

	/* cpy file data onto buffer */
	if (opts->verbose)
		status_init("reading program");
	FILE *f;
	if (!strcmp(opts->path, "-")) {
		f = stdin;
	} else {
		if ((f = fopen(opts->path, "r")) == NULL)
			die("lbf: error: '%s': cannot open:", opts->path);
	}

	usize i = 0;
	for (int c = 0; (c = fgetc(f)) != EOF; ++i) {
		/* printing info too often will
		 * slow down reading significantly */
		if (opts->verbose && !(c % 128))
			status_update("reading program", c, STATUS_UNKNOWN);

		/* check if buffer is big enough
		 * for data (including null terminator!!) */
		if ((i + 1) >= sz)
			program_data = realloc(program_data, (sz *= 2));

		program_data[i] = c;
	}

	program_data[i + 1] = '\0';
	if (f != stdin) fclose(f);
	if (opts->verbose)
		status_complete("reading program");

	parse(opts, program_data, head);

	if (opts->fopt_enable_command_squashing)
		optimize_squash(opts, head);
	if (opts->fopt_enable_nullify_command)
		optimize_nloops(opts, head);
	if (opts->fopt_enable_scan_command)
		optimize_sloops(opts, head);

	free(program_data);
	return 0;
}
