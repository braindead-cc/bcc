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
	char *program_data = (char*) calloc(st.st_size + 1, sizeof(char));

	FILE *f;
	if ((f = fopen(path, "r")) == NULL)
		die("lbfi: cannot open %s:", path);

	/* copy file data onto buffer */
	int c = 0;
	usize i = 0;
	for (; (c = fgetc(f)) != EOF; ++i)
		program_data[i] = c;
	program_data[i + 1] = '\0';

	/* parse instructions -- program = head node */
	struct Instruction *program = malloc(sizeof(struct Instruction));
	program == NULL && die("lbfi: error: cannot allocate tape:");
	parse(opts, program_data, program);


	/* TODO: warnings, optimizations */
	/* execute instructions */
	struct Tape *tape = malloc(1 * sizeof(struct Tape));
	if (tape == NULL)
		die("lbfi: error: cannot allocate memory for tape:");
	tape->tp_size = opts->fopt_initial_tape_size;
	tape->cells = calloc(tape->tp_size, sizeof(u8));
	if (tape->cells == NULL)
		die("lbfi: error: cannot allocate memory for tape:");
	tape->pointer = 0;

	bf_init(opts, tape);
	struct Instruction *current = program;
	for (usize depth = 0, i = 0; current->next != NULL;
			current = current->next, ++i) {
		switch (current->command) {
		case COMMAND_CELL_NULLIFY:
			bf_cell_nullify(tape);
			break;
		case COMMAND_CELL_INC:
			bf_cell_inc(tape, current->repeat);
			break;
		case COMMAND_CELL_DEC:
			bf_cell_dec(tape, current->repeat);
			break;
		case COMMAND_PTR_MOV_INIT:
			bf_ptr_mov_init(tape);
			break;
		case COMMAND_PTR_MOV_L:
			bf_ptr_mov_l(tape, current->repeat);
			break;
		case COMMAND_PTR_MOV_R:
			bf_ptr_mov_r(tape, current->repeat);
			break;
		case COMMAND_LOOP_START:
			if (tape->cells[tape->pointer] == 0) {
				current = current->next;
				depth = 1;
				while (depth > 0) {
					current = current->next;
					if (current->command == COMMAND_LOOP_START)
						++depth;
					else if (current->command == COMMAND_LOOP_END)
						--depth;
				}
			}
			break;
		case COMMAND_LOOP_END:
			if (tape->cells[tape->pointer] != 0) {
				current = current->prev;
				depth = 1;
				while (depth > 0) {
					current = current->prev;
					if (current->command == COMMAND_LOOP_START)
						--depth;
					else if (current->command == COMMAND_LOOP_END)
						++depth;
				}
			}
			break;
		case COMMAND_READ_STDIN:
			bf_read_stdin(tape);
			break;
		case COMMAND_PRINT_STDOUT:
			bf_print_stdout(tape);
			break;
		case COMMAND_PRINT_STDERR:
			bf_print_stderr(tape);
			break;
		case COMMAND_PRINT_DEBUG:
			bf_print_debug(tape);
			break;
		case COMMAND_SCAN_L:
			bf_scan_l(tape);
			break;
		case COMMAND_SCAN_R:
			bf_scan_r(tape);
			break;
		case COMMAND_SUICIDE:
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
	struct Instruction *s;
	for (s = program; s->next != NULL; s = s->next)
		if (s->prev) free(s->prev);
	if (s) free(s);

	return 0;
}
