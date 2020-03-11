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
static void get_cell_min_max(struct Options *opts);

/* allocate tape and get cell max/min */
void
bf_init(struct Options *opts, struct Tape *tape)
{
	tape          = malloc(1 * sizeof(struct Tape));
	if (tape == NULL)
		die("lbfi: error: cannot allocate memory for tape:");

	tape->cells   = malloc(opts->fopt_initial_tape_size * sizeof(i64));
	if (tape->cells == NULL)
		die("lbfi: error: cannot allocate memory for tape:");

	tape->tp_size = opts->fopt_initial_tape_size;
	tape->pointer = 0;

	get_cell_min_max(opts);
}

static void
get_cell_min_max(struct Options *opts)
{
	if (opts->fopt_cell_size == 8) {
		if (opts->fopt_cell_signed)
			cell_max = 127;
		else
			cell_max = 255;
	} else if (opts->fopt_cell_size == 16) {
		if (opts->fopt_cell_signed)
			cell_max = 32767;
		else
			cell_max = 65535;
	} else if (opts->fopt_cell_size == 32) {
		if (opts->fopt_cell_signed)
			cell_max = 2147483647;
		else
			cell_max = 4294967295;
	}

	if (opts->fopt_cell_signed)
		cell_min = (-cell_max) - 1;
	else
		cell_min = 0;
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
	tape->cells[tape->pointer] += amount;
}

void
bf_cell_dec(struct Tape *tape, usize amount)
{
	tape->cells[tape->pointer] -= amount;
}

void
bf_ptr_mov_init(struct Tape *tape)
{
	tape->pointer = 0;
}

void
bf_ptr_mov_l(struct Tape *tape, usize amount)
{
	/* TODO: handle segfault */
	tape->pointer -= amount;
}

void
bf_ptr_mov_r(struct Tape *tape, usize amount)
{
	/* TODO: handle segfault */
	tape->pointer += amount;
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
	free(tape->cells);
	free(tape);
	exit(0);

	/* TODO: free opts, program_data, etc */
}
