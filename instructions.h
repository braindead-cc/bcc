#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

enum Registers {
	REG_A,
	REG_B,
	REG_C,
	REG_D,
	REG_E,
	REG_F,
}

enum Command {
	COMMAND_CELL_NULLIFY,  /* '*' -- set cell to 0 */
	COMMAND_CELL_INC,      /* '+' -- increment cell */
	COMMAND_CELL_DEC,      /* '-' -- decrement cell */
	COMMAND_PTR_MOV_INIT,  /* '^' -- move ptr to cell 0 */
	COMMAND_PTR_MOV_L,     /* '<' -- move ptr to the left */
	COMMAND_PTR_MOV_R,     /* '>' -- move ptr to the right */
	COMMAND_LOOP_START,    /* '[' -- start bf loop */
	COMMAND_LOOP_END,      /* ']' -- end bf loop */
	COMMAND_COND_START,    /* '(' -- begin conditional (like loop) */
	COMMAND_COND_END,      /* ')' -- end conditional */
	COMMAND_READ_STDIN,    /* ',' -- replace cell with char from stdin */
	COMMAND_PRINT_STDOUT,  /* '.' -- output current cell value to stdout */
	COMMAND_PRINT_STDERR,  /* '&' -- output current cell value to stderr */
	COMMAND_PRINT_DEBUG,   /* '#' -- output debugging information */
	COMMAND_GET_REG,       /* '%' -- set cell to value of register */
	COMMAND_SET_REG_A,     /* 'A' -- set regA to value of cell */
	COMMAND_SET_REG_B,     /* 'B' -- set regB to value of cell */
	COMMAND_SET_REG_C,     /* 'C' -- set regC to value of cell */
	COMMAND_SET_REG_D,     /* 'D' -- set regD to value of cell */
	COMMAND_SET_REG_E,     /* 'E' -- set regE to value of cell */
	COMMAND_SET_REG_F,     /* 'F' -- set regF to value of cell */
	COMMAND_SCAN_L,        /* '{' -- eq to '[<]' */
	COMMAND_SCAN_R,        /* '}' -- eq to '[>]' */
	COMMAND_SUICIDE,       /* '@' -- exit with exit code of cell value */

	COMMAND_UNKNOWN,
}

struct Instruction {
	enum Command command;

	/* context */
	usize line;
	usize char;

	/* number of times to repeat command */
	usize repeat;

	/* we're storing Instructions as a linked list
	 * so that it becomes super easy to remove
	 * Instructions in case we can optimize it away */
	struct Instruction *prev, *next;
}

#endif