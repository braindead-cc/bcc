#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "exec.h"
#include "instructions.h"
#include "lbfi.h"
#include "parser.h"
#include "types.h"
#include "util.h"

int
lbfi_main(char *path, struct Options *opts)
{
	/* get size of file so we can allocate a buffer for it */
	struct stat st;
	if (lstat(path, &st) == -1)
		die("lbfi: error: cannot stat %s:", path);
	char *program_data = (char*) malloc(st.st_size + 1 * sizeof(char));

	FILE *f;
	if ((f = fopen(path, "r")) == NULL)
		die("lbfi: cannot open %s:", path);

	/* copy file data onto buffer */
	int c = 0;
	usize i = 0;
	for (; (c = fgetc(f)) != EOF; ++i)
		program_data[i] = c;
	program_data[i + 1] = '\0';

	/* TODO: optimizations */
	/* execute instructions */
	struct Tape *tape = malloc(1 * sizeof(struct Tape));
	if (tape == NULL)
		die("lbfi: error: cannot allocate memory for tape:");
	tape->tp_size = opts->fopt_initial_tape_size;
	tape->cells = calloc(tape->tp_size, sizeof(u8));
	if (tape->cells == NULL)
		die("lbfi: error: cannot allocate memory for tape:");
	tape->pointer = 0;
	printf("DEBUG: tape size: %lld\n", tape->tp_size);

	bf_init(opts, tape);
	for (usize depth = 0, i = 0; i < st.st_size; ++i) {
		switch (program_data[i]) {
		case '*':
			bf_cell_nullify(tape);
			break;
		case '+':
			bf_cell_inc(tape, 1);
			break;
		case '-':
			bf_cell_dec(tape, 1);
			break;
		case '^':
			bf_ptr_mov_init(tape);
			break;
		case '<':
			bf_ptr_mov_l(tape, 1);
			break;
		case '>':
			bf_ptr_mov_r(tape, 1);
			break;
		case '[':
			if (tape->cells[tape->pointer] == 0) {
				++i;
				depth = 1;
				while (depth > 0) {
					++i;
					if (program_data[i] == '[')
						++depth;
					else if (program_data[i] == ']')
						--depth;
				}
			}
			break;
		case ']':
			if (tape->cells[tape->pointer] != 0) {
				--i;
				depth = 1;
				while (depth > 0) {
					--i;
					if (program_data[i] == '[')
						--depth;
					else if (program_data[i] == ']')
						++depth;
				}
				--i;
			}
			break;
		case ',':
			bf_read_stdin(tape);
			break;
		case '.':
			bf_print_stdout(tape);
			break;
		case '&':
			bf_print_stderr(tape);
			break;
		case '#':
			bf_print_debug(tape);
			break;
		case '{':
			bf_scan_l(tape);
			break;
		case '}':
			bf_scan_r(tape);
			break;
		case '@':
			goto cleanup;
			break;
		default:
			break;
		}
	}

cleanup:
	/* cleanup */
	bf_suicide(tape);
	if (opts) free(opts);

	return 0;
}
