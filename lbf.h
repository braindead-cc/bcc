#ifndef LBF_H
#define LBF_H

#include "bool.h"
#include "instructions.h"
#include "types.h"

/* compiler backends */
enum bparams {
	BOPT_C,      /* C backend */
	BOPT_ARM,    /* ARM assembly backend */
	BOPT_LLVM,   /* LLVM backend */
	BOPT_RUST,   /* Rust backend */
	BOPT_V,      /* V backend */
	BOPT_GO,     /* Go backend */
	BOPT_QBE,    /* QBE backend */
};

struct Options {
	bool debug;
	bool verbose;
	enum bparams backend;

	/* options enabled with -f (e.g. -fenable-dbg) */
	bool fopt_enable_dbg_command;       /* enable # command */
	bool fopt_enable_lbf_std;           /* enable lbf extensions */

	/* optimizations (also used with -f) */
	bool fopt_enable_nullify_command;   /* [-] => * */
	bool fopt_enable_scan_command;      /* [<] => {, [>] => } */
	bool fopt_enable_command_squashing; /* +++ => 3+ */

	/* warnings */
	bool wopt_error;         /* treat all warnings as error */
	bool wopt_comments;      /* warn on encountering comments */
	bool wopt_long_lines;    /* warn on lines longer than 80 */
	bool wopt_dead_code;     /* warn on dead code */
	bool wopt_ignore_dbg;    /* warn on ignored # command */
	bool wopt_tape_overflow; /* warn on moving to cell -1 */
	bool wopt_all;

	/* other options */
	u8   fopt_cell_size;     /* cell size (8, 16, 32, or 64) */
	bool fopt_cell_signed;   /* enable signed cells */

	/* number of cells to print before+after 
	 * pointer on debug instruction */
	u64 fopt_debug_context;

	/* initial amount of memory allocated for tape */
	u64 fopt_initial_tape_size;
};

#endif
