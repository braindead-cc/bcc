#include <stdio.h>
#include <stdlib.h>

#include "exec.h"
#include "instructions.h"
#include "lbfi.h"
#include "parser.h"
#include "types.h"
#include "util.h"

int
lbfi_main(char *path, struct Options *opts)
{
	/* TODO: allocate on demand */
	char *program_data = (char*) malloc(128000 * sizeof(char));
	if (program_data == NULL)
		die("lbfi: error: cannot read brainfsck code:");

	/* copy file data onto buffer */
	usize i = 0;
	for (int c = 0; (c = fgetc(stdin)) != EOF && i < 128000; ++i)
		program_data[i] = c;
	program_data[i + 1] = '\0';

	struct Instruction *head = malloc(sizeof(struct Instruction));
	if (head == NULL)
		die("lbfi: error: cannot allocate memory:");
	parse(opts, program_data, head);

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
	if (opts->debug) debug("tape size = %lld", tape->tp_size);

	bf_init(opts, tape);
	struct Instruction *cur = head;
	for (usize depth = 0; cur->next != NULL; cur = cur->next) {
		switch (cur->command) {
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
			bf_ptr_mov_r(opts, tape, 1);
			break;
		case '[':
			if (tape->cells[tape->pointer] == 0) {
				cur = cur->next;
				depth = 1;
				while (depth > 0) {
					cur = cur->next;
					if (cur->command == '[')
						++depth;
					else if (cur->command == ']')
						--depth;
				}
			}
			break;
		case ']':
			if (tape->cells[tape->pointer] != 0) {
				cur = cur->prev;
				depth = 1;
				while (depth > 0) {
					cur = cur->prev;
					if (cur->command == '[')
						--depth;
					else if (cur->command == ']')
						++depth;
				}
				cur = cur->prev;
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
	for (struct Instruction *x = head->next; x->next != NULL; x = x->next)
		if (x->prev != NULL) free(x->prev);
	if (opts) free(opts);

	return 0;
}
