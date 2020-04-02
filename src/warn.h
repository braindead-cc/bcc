/*
 * warn messages and warn codes
 */

#ifndef WARNINGS_H
#define WARNINGS_H

#include "options.h"
#include "types.h"

enum {
	W_LONG_LINES = 0,
	W_DEAD_CODE = 1,
	W_IGNORE_DBG = 2,
};

void warn(struct Options *opts, usize line, usize column, usize warning);

#endif
