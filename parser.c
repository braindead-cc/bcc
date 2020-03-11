#include <stdlib.h>

#include "instructions.h"
#include "lbf.h"
#include "parser.h"

void
parse(char *program_data, struct Instruction *program)
{
	usize line = 0;
	usize column = 0;

	struct Instruction *last = program;

	for (usize i = 0; program_data[i]; ++i) {
		if (program_data[i] == '\n') {
			++line;
			continue;
		} else {
			++column;
		}

		struct Instruction *inst = malloc(sizeof(struct Instruction));
		inst->line    = line;
		inst->column  = column,
		inst->repeat  = 1;
		inst->prev    = last;
		last->next    = inst;

		switch (program_data[i]) {
		case '*':
			inst->command = COMMAND_CELL_NULLIFY;
			break;
		case '+':
			inst->command = COMMAND_CELL_INC;
			break;
		case '-':
			inst->command = COMMAND_CELL_DEC;
			break;
		case '^':
			inst->command = COMMAND_PTR_MOV_INIT;
			break;
		case '<':
			inst->command = COMMAND_PTR_MOV_L;
			break;
		case '>':
			inst->command = COMMAND_PTR_MOV_R;
			break;
		case '[':
			inst->command = COMMAND_LOOP_START;
			break;
		case ']':
			inst->command = COMMAND_LOOP_END;
			break;
		case ',':
			inst->command = COMMAND_READ_STDIN;
			break;
		case '.':
			inst->command = COMMAND_PRINT_STDOUT;
			break;
		case '&':
			inst->command = COMMAND_PRINT_STDERR;
			break;
		case '#':
			inst->command = COMMAND_PRINT_DEBUG;
			break;
		case '{':
			inst->command = COMMAND_SCAN_L;
			break;
		case '}':
			inst->command = COMMAND_SCAN_R;
			break;
		case '@':
			inst->command = COMMAND_SUICIDE;
			break;
		default:
			continue;
			break;
		}

		last = inst;
	}
}
