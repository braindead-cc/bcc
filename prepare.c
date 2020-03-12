/*
 * read, lex, parse, optimize, and lint
 * branfsck code.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "instructions.h"
#include "lbf.h"
#include "parser.h"
#include "prepare.h"
#include "util.h"

usize
prepare(struct Options *opts, struct Instruction *head)
{
	/* TODO: allocate on demand */
	char *program_data = malloc(128000 * sizeof(char));
	if (program_data == NULL)
		die("lbf: error: cannot read brainfsck code:");

	/* cpy file data onto buffer */
	usize i = 0;
	for (int c = 0; (c = fgetc(stdin)) != EOF; ++i)
		program_data[i] = c;
	program_data[i + 1] = '\0';

	parse(opts, program_data, head);

	/* TODO: optimize code */
	/* TODO: lint code */

	free(program_data);
	return 0;
}
