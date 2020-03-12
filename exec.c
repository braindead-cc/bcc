/*
 * execute bf instructions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec.h"
#include "instructions.h"
#include "lbf.h"
#include "utf.h"
#include "util.h"

static u32 cell_max = 0;
static u32 cell_min = 0;

void
bf_init(struct Options *opts, struct Tape *tape)
{
}

void
bf_cell_nullify(struct Tape *tape)
{
	tape->cells[tape->pointer] = 0;
}

void
bf_cell_inc(struct Tape *tape, usize amount)
{
	/* TODO: wrapping */
	u8 tmp = ((u8) tape->cells[tape->pointer]) + amount;
	tape->cells[tape->pointer] = tmp;
}

void
bf_cell_dec(struct Tape *tape, usize amount)
{
	u8 tmp = ((u8) tape->cells[tape->pointer]) - amount;
	tape->cells[tape->pointer] = tmp;
}

void
bf_ptr_mov_init(struct Tape *tape)
{
	tape->pointer = 0;
}

void
bf_ptr_mov_l(struct Tape *tape, usize amount)
{
	/* check for overflow */
	u64 tmp = tape->pointer - amount;
	if (tmp > tape->pointer) {
		/* ignore */
		/* TODO: add -Wtape-overflow */
	} else {
		tape->pointer -= amount;
	}
}

void
bf_ptr_mov_r(struct Options *opts, struct Tape *tape, usize amount)
{
	u64 tmp = tape->pointer + amount;
	if (tmp >= tape->tp_size) {
		if (opts->debug)
			debug("reallocating %lld for tape", tape->tp_size * 2);
		tape->tp_size = tape->tp_size * 2;
		tape->cells   = realloc(tape->cells, tape->tp_size);
		if (tape->cells == NULL)
			die("lbf: error: cannot allocate memory for tape:");
	} else {
		tape->pointer += amount;
	}
}

void
bf_read_stdin(struct Tape *tape)
{
	int c = fgetc(stdin);
	tape->cells[tape->pointer] = c == EOF ? c : 0;
}

void
bf_print(struct Tape *tape, FILE *f)
{
	if (tape->cells[tape->pointer] < 256) {
		fputc(tape->cells[tape->pointer], f);
	} else {
		char buf[runelen((Rune) tape->cells[tape->pointer])];
		runetochar(&buf, (Rune*) &tape->cells[tape->pointer]);
		fprintf(f, "%s", (char*) &buf);
	}
}

void
bf_print_stdout(struct Tape *tape)
{
	bf_print(tape, stdout);
}

void
bf_print_stderr(struct Tape *tape)
{
	bf_print(tape, stderr);
}

void
bf_print_debug(struct Tape *tape)
{
	/* TODO: define -fdebug-context
	 * to specify how many cells should be printed */
	for (usize i = 0; i < tape->tp_size; ++i) {
		/* TODO: beautify output */
		fprintf(stdout, "DEBUG: %i = %i\n", i, tape->cells[i]);
	}
}

void
bf_scan_l(struct Tape *tape)
{
	tape->pointer -= (i32) ((void *) &tape->cells[tape->pointer]
			- memrchr(tape->cells, 0, tape->cells[tape->pointer + 1]));
}

void
bf_scan_r(struct Tape *tape)
{
	tape->pointer += (i32) (memchr(&tape->cells[tape->pointer], 0, tape->tp_size)
			- (void *)(&tape->cells[tape->pointer]));
}

void
bf_suicide(struct Tape *tape)
{
	if (tape->cells)
		free(tape->cells);
	if (tape) free(tape);

	/* TODO: free opts, program_data, etc */
}
