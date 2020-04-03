#ifndef INTERP_H
#define INTERP_H

#include "instructions.h"
#include "options.h"

int interp_main(struct Options *opts, struct Instruction *head);

#endif
