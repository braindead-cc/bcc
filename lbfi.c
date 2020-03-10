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

	/* parse instructions */
	struct Instruction *program = calloc(st.st_size,
			sizeof(struct Instruction));
	parse(program_data, program);


	/* TODO: warnings, optimizations */
	/* execute instructions */
	struct Tape *tape;
	bf_init(tape);

	struct Instruction *current = &program[0];
	for (; current->next != NULL; current = current->next) {
		switch (current->command) {
		case COMMAND_CELL_NULLIFY:
			bf_cell_nullify();
			break;
		case COMMAND_CELL_INC:
			bf_cell_inc(current->repeat);
			break;
		case COMMAND_CELL_DEC:
			bf_cell_dec(current->repeat);
			break;
		case COMMAND_PTR_MOV_INIT:
			bf_ptr_mov_init();
			break;
		case COMMAND_PTR_MOV_L:
			bf_ptr_mov_l(current->repeat);
			break;
		case COMMAND_PTR_MOV_R:
			bf_ptr_mov_r(current->repeat);
			break;
		case COMMAND_LOOP_START:
			if (*(tape->pointer) == 0) {
				current = current->next;
				usize depth = 0;
				while (depth > 0 || current->command != COMMAND_LOOP_END) {
					if (current->command == COMMAND_LOOP_START)
						++depth;
					else if (current->command == COMMAND_LOOP_END)
						--depth;
					current = current->next;
				}
			}
			break;
		case COMMAND_LOOP_END:
			if (*(tape->pointer) != 0) {
				current = current->prev;
				usize depth = 0;
				while (depth > 0 || current->command != COMMAND_LOOP_START) {
					if (current->command == COMMAND_LOOP_END)
						++depth;
					else if (current->command == COMMAND_LOOP_START)
						--depth;
					current = current->prev;
				}
				current = current->prev;
			}
			break;
		case COMMAND_READ_STDIN:
			bf_read_stdin();
			break;
		case COMMAND_PRINT_STDOUT:
			bf_print_stdout();
			break;
		case COMMAND_PRINT_STDERR:
			bf_print_stderr();
			break;
		case COMMAND_PRINT_DEBUG:
			bf_print_debug();
			break;
		case COMMAND_SCAN_L:
			bf_scan_l();
			break;
		case COMMAND_SCAN_R:
			bf_scan_r();
			break;
		case COMMAND_SUICIDE:
			bf_suicide();
			break;
		default:
			break;
		}
	}

	/* cleanup */
	bf_suicide();

	return 0;
}
