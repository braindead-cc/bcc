#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instructions.h"
#include "lbf.h"
#include "parser.h"

void
parse(struct Options *opts, char *program_data, struct Instruction *program)
{
	usize len  = strlen(program_data);
	usize line = 0;
	usize column = 0;

	struct Instruction *last = program;

	for (usize i = 0; i < len; ++i) {
		if (opts->verbose)
			fprintf(stderr, "\rparsing char %d / %d... ", i, len);
		program_data[i] == '\n' ? ++line : ++column;

		usize command = COMMAND_UNKNOWN;

		switch (program_data[i]) {
		case '*':
			command = COMMAND_CELL_NULLIFY;
			break;
		case '+':
			command = COMMAND_CELL_INC;
			break;
		case '-':
			command = COMMAND_CELL_DEC;
			break;
		case '^':
			command = COMMAND_PTR_MOV_INIT;
			break;
		case '<':
			command = COMMAND_PTR_MOV_L;
			break;
		case '>':
			command = COMMAND_PTR_MOV_R;
			break;
		case '[':
			command = COMMAND_LOOP_START;
			break;
		case ']':
			command = COMMAND_LOOP_END;
			break;
		case ',':
			command = COMMAND_READ_STDIN;
			break;
		case '.':
			command = COMMAND_PRINT_STDOUT;
			break;
		case '&':
			command = COMMAND_PRINT_STDERR;
			break;
		case '#':
			command = COMMAND_PRINT_DEBUG;
			break;
		case '{':
			command = COMMAND_SCAN_L;
			break;
		case '}':
			command = COMMAND_SCAN_R;
			break;
		case '@':
			command = COMMAND_SUICIDE;
			break;
		default:
			continue;
			break;
		}

		struct Instruction *inst = malloc(sizeof(struct Instruction));
		inst->command = command;
		inst->line    = line;
		inst->column  = column,
		inst->repeat  = 1;
		inst->prev    = last;
		last->next    = inst;

		last = inst;
	}

	if (opts->verbose) fprintf(stderr, "done \n");
}
