/*
 * execute bf instructions
 */

#include <stdio.h>
#include <stdlib.h>

#include "exec.h"
#include "instruction.h"
#include "lbf.h"
#include "utf.h"

static u32 cell_max = 0;
static u32 cell_min = 0;
static void get_cell_min_max(void);

/* allocate tape and get cell max/min */
void
bf_init(void)
{
	tape          = (struct Tape*) calloc(1, sizeof(struct Tape));
	tape->cells   = (i64*) calloc(tape_initial_size, sizeof(i64));
	tape->tp_size = tape_initial_size;
	tape->pointer = &tape->cells[0];

	get_cell_min_max();
}

static void
get_cell_min_max(void)
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
bf_cell_nullify(void)
{
	*(tape->pointer) = 0;
}

void
bf_cell_inc(usize amount)
{
	/* TODO: wrapping */
	*(tape->pointer) += amount;
}

void
bf_cell_dec(usize amount)
{
	*(tape->pointer) -= amount;
}

void
bf_ptr_mov_init(void)
{
	tape->pointer = &tape->cells[0];
}

void
bf_ptr_mov_l(void)
{
	/* TODO: handle segfault */
	--tape->pointer;
}

void
bf_ptr_mov_r(void)
{
	/* TODO: handle segfault */
	++tape->pointer;
}

void
bf_read_stdin(void)
{
	int c = fgetc(stdin);
	*(tape->pointer) = c == EOF ? c : 0;
}

void
bf_print(FILE *f)
{
	*(tape->pointer) < 256 ? fputc(*(tape->pointer), f) : {
		char buf[runelen((Rune*) tape->pointer)];
		runetochar(&buf, (Rune*) tape->pointer);
		fprintf(f, "%s", &buf);
	};
}

void
bf_print_stdout(void)
{
	bf_print(stdout);
}

void
bf_print_stderr(void)
{
	bf_print(stderr);
}

void
bf_print_debug(void)
{
	/* TODO: define -fdebug-context
	 * to specify how many cells should be printed */
	for (usize i = 0; i < tape->tp_size; ++i) {
		/* TODO: beautify output */
		fprintf(stdout, "DEBUG: %i = %i\n", i, tape->cells[i]);
	}
}

void
bf_scan_l(void)
{
	tape->pointer -= (i32) ((void *)*(tape->pointer)
			- memrchr(tape->cells, 0, tape->pointer + 1));
}

void
bf_scan_r(void)
{
	tape->pointer += (i32) (memchr(tape->pointer, 0, tape->tp_size)
			- (void *)(tape->pointer));
}

void
bf_suicide(void)
{
	free(tape->cells);
	free(tape);
	exit(0);

	/* TODO: free opts, program_data, etc */
}
