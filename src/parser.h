#ifndef PARSE_H
#define PARSE_H
#include "instructions.h"
#include "options.h"

void parse(struct Options *opts, char *program_data,
	struct Instruction *program);

#endif
