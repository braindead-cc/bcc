#ifndef EXEC_H
#define EXEC_H

#include "types.h"

/* initial size of tape */
const usize tape_initial_size = 128;

struct Tape {
	/* number of cells */
	u64 tp_size;

	/* use i32, wrap later if
	 * user-defined cell size is lower */
	i32 *cells;

	/* cell pointer */
	i32 *pointer;
}

struct Tape *tape;

/* prototypes */
void bf_init(void);
void bf_cell_nullify(void);
void bf_cell_inc(void);
void bf_cell_dec(void);
void bf_ptr_mov_init(void);
void bf_ptr_mov_l(void);
void bf_ptr_mov_r(void);
void bf_read_stdin(void);
void bf_print(FILE *f);
void bf_print_stdout(void);
void bf_print_stderr(void);
void bf_print_debug(void);
void bf_get_reg(void);
void bf_set_reg(void);
void bf_scan_l(void);
void bf_scan_r(void);
void bf_suicide(void);

#endif
