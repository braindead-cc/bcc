#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include "types.h"

struct Instruction {
	char command;

	/* context */
	usize line;
	usize column;

	/* number of times to repeat command */
	usize repeat;

	/* we're storing Instructions as a linked list
	 * so that it becomes super easy to remove
	 * Instructions in case we can optimize it away */
	struct Instruction *prev, *next;
};

#endif
