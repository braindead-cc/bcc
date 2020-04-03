#ifndef OPTIONS_H
#define OPTIONS_H

#include "bool.h"
#include "instructions.h"
#include "types.h"

/* compiler backends */
enum bparams {
	BOPT_C,        /* C backend */
	BOPT_ARM,      /* ARM assembly backend */
	BOPT_LLVM,     /* LLVM backend */
	BOPT_RUST,     /* Rust backend */
	BOPT_V,        /* V backend */
	BOPT_GO,       /* Go backend */
	BOPT_QBE,      /* QBE backend */
};

enum eof_values {
	EOF_MINUS_1,   /* -1 */
	EOF_ZERO,      /*  0 */
	EOF_NO_CHANGE, /* no change made to cell */
};

struct Options {
	char *path; /* input file */

	bool debug;
	bool verbose;
	enum bparams backend;

	/* options enabled with -f (e.g. -fenable-dbg) */
	bool fopt_enable_dbg_command;       /* enable # command */

	/* optimizations (also used with -f) */
	bool fopt_enable_nullify_command;   /* [-] => * */
	bool fopt_enable_scan_command;      /* [<] => {, [>] => } */
	bool fopt_enable_command_squashing; /* +++ => 3+ */

	/* warnings */
	bool wopt_long_lines;    /* warn on lines longer than 80 */
	bool wopt_dead_code;     /* warn on dead code */
	bool wopt_ignore_dbg;    /* warn on ignored # command */

	/* other options */
	u8   fopt_cell_size;     /* cell size (8, 16, 32, or 64) */
	bool fopt_cell_signed;   /* enable signed cells */

	/* initial amount of memory allocated for tape */
	u64 fopt_initial_tape_size;

	bool fopt_disable_dynamic_alloc;

	/* comment char */
	char fopt_comment_char;

	/* EOF char (-1 for no change) */
	enum eof_values fopt_eof_char;
};

#endif
