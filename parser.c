#include "instructions.h"

void
parse(void)
{
	usize line = 0;
	usize char = 0;

	for (usize i = 0; program_data[i]; ++i) {
		program_data[i] == '\n' ? ++line, continue : ++char;

		struct Instruction inst = {
			COMMAND_UNKNOWN,
			line, char,
			1,
			NULL, NULL
		};

		switch (program_data[i]) {
		case '*':
			inst.command = COMMAND_CELL_NULLIFY;
			break;
		case '+':
			inst.command = COMMAND_CELL_INC;
			break;
		case '-':
			inst.command = COMMAND_CELL_DEC;
			break;
		case '^':
			inst.command = COMMAND_PTR_MOV_INIT;
			break;
		case '<':
			inst.command = COMMAND_PTR_MOV_L;
			break;
		case '>':
			inst.command = COMMAND_PTR_MOV_R;
			break;
		case '[':
			inst.command = COMMAND_LOOP_START;
			break;
		case ']':
			inst.command = COMMAND_LOOP_END;
			break;
		case ',':
			inst.command = COMMAND_READ_STDIN;
			break;
		case '.':
			inst.command = COMMAND_PRINT_STDOUT;
			break;
		case '&':
			inst.command = COMMAND_PRINT_STDERR;
			break;
		case '#':
			inst.command = COMMAND_PRINT_DEBUG;
			break;
		case '{':
			inst.command = COMMAND_SCAN_L;
			break;
		case '}':
			inst.command = COMMAND_SCAN_R;
			break;
		case '@':
			inst.command = COMMAND_SUICIDE;
			break;
		default:
			continue;
			break;
		}

		if (i != 0)
			inst.prev = &program[i - 1];
		if (program_data[i + 1] != '\0')
			inst.next = &program[i + 1];

		program[i] = inst;
	}
}
