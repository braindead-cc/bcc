#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>

#include "lbf.h"
#include "types.h"

struct Tape {
	/* number of cells */
	u64 tp_size;

	u8 *cells;

	/* cell pointer */
	u8 pointer;
};

/* prototypes */
void bf_init(struct Options *opts, struct Tape *tape);
void bf_cell_nullify(struct Tape *tape);
void bf_cell_inc(struct Tape *tape, usize amount);
void bf_cell_dec(struct Tape *tape, usize amount);
void bf_ptr_mov_init(struct Tape *tape);
void bf_ptr_mov_l(struct Tape *tape, usize amount);
void bf_ptr_mov_r(struct Tape *tape, usize amount);
void bf_read_stdin(struct Tape *tape);
void bf_print(struct Tape *tape, FILE *f);
void bf_print_stdout(struct Tape *tape);
void bf_print_stderr(struct Tape *tape);
void bf_print_debug(struct Tape *tape);
void bf_scan_l(struct Tape *tape);
void bf_scan_r(struct Tape *tape);
void bf_suicide(struct Tape *tape);

#endif
