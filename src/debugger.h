#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "instructions.h"
#include "options.h"

void debugger_main(struct Options *opts, struct Instruction *head);

#endif
