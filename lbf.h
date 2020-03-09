#ifndef LBF_H
#define LBF_H

#include "bool.h"
#include "instructions.h"

/* compiler backends */
enum bparams {
	BOPT_C,      /* C backend */
	BOPT_SHELL,  /* POSIX sh backend */
	BOPT_LLVM,   /* LLVM backend */
	BOPT_RUST,   /* Rust backend */
	BOPT_V,      /* V backend */
	BOPT_GO,     /* Go backend */
	BOPT_QBE,    /* QBE backend */
}

struct Options {
	bool verbose;
	enum bparams backend;
	char *ofile; /* output file */

	/* options enabled with -f (e.g. -fenable-dbg) */
	bool fopt_enable_dbg_command;       /* enable # command */
	bool fopt_enable_lbf_std;           /* enable lbf extensions */

	/* optimizations (also used with -f) */
	bool fopt_enable_nullify_command;   /* [-] => * */
	bool fopt_enable_scan_command;      /* [<] => {, [>] => } */
	bool fopt_enable_command_squashing; /* +++ => 3+ */

	/* warnings */
	bool wopt_error;       /* treat all warnings as error */
	bool wopt_comments;    /* warn on encountering comments */
	bool wopt_long_lines;  /* warn on lines longer than 80 */
	bool wopt_dead_code;   /* warn on dead code */
	bool wopt_ignored_dbg; /* warn on ignored # command */
	bool wopt_all;
}

struct Options *opts;
struct Instruction *program;

#endif
