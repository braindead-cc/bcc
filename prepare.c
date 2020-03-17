/*
 * read, lex, parse, optimize, and lint
 * branfsck code.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "instructions.h"
#include "lbf.h"
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
	/* TODO: allocate on demand */
	char *program_data = malloc(128000 * sizeof(char));
	if (program_data == NULL)
		die("lbf: error: cannot read brainfsck code:");

	/* cpy file data onto buffer */
	if (opts->verbose)
		status_init("reading program");
	usize i = 0;
	for (int c = 0; (c = fgetc(stdin)) != EOF; ++i) {
		/* printing info too often will
		 * slow down reading significantly */
		if (opts->verbose && !(c % 128))
			status_update("reading program", c, STATUS_UNKNOWN);
		program_data[i] = c;
	}
	program_data[i + 1] = '\0';
	if (opts->verbose)
		status_complete("reading program");

	parse(opts, program_data, head);

	if (opts->fopt_enable_command_squashing)
		optimize_squash(opts, head);
	if (opts->fopt_enable_nullify_command)
		optimize_nloops(opts, head);
	if (opts->fopt_enable_scan_command)
		optimize_sloops(opts, head);

	if (opts->debug) {
		fprintf(stderr, "lbf: debug: instructions after optimizing: ");
		for (struct Instruction *i = head->next; i != NULL;
				i = i->next) {
			if (i->repeat > 1)
				fprintf(stderr, "%d%c", i->repeat,
						(char) i->command);
			else
				fprintf(stderr, "%c", i->command);
		}
		fputc('\n', stderr);
	}

	free(program_data);
	return 0;
}
