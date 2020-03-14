#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instructions.h"
#include "lbfi.h"
#include "parser.h"
#include "prepare.h"
#include "tape.h"
#include "types.h"
#include "util.h"

int
lbfi_main(struct Options *opts, struct Instruction *head)
{
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

	struct Instruction *cur = head;
	for (usize depth = 0; cur->next != NULL; cur = cur->next) {
		switch (cur->command) {
		case '*':
			tape->cells[tape->pointer] = 0;
			break;
		case '+':
			/* TODO: custom cell size, signed cells */
			tape->cells[tape->pointer] += cur->repeat;
			break;
		case '-':
			tape->cells[tape->pointer] -= cur->repeat;
			break;
		case '^':
			tape->pointer = 0;
			break;
		case '<':
			{
			u8 tmp = tape->pointer - cur->repeat;
			if (tmp > tape->pointer) {
				/* ignore */
				/* TODO: implement -Wtape-overflow */
			} else {
				tape->pointer -= cur->repeat;
			}
			}
			break;
		case '>':
			{
			u8 tmp = tape->pointer + cur->repeat;
			if (tmp >= tape->tp_size) {
				tape->tp_size *= 2;
				if (opts->debug)
					debug("reallocating %lld for tape",
							tape->tp_size);
				tape->cells = realloc(tape->cells,
						tape->tp_size);
				if (tape->cells == NULL)
					die("lbf: error: cannot allocate memory for tape:");
			} else {
				tape->pointer += cur->repeat;
			}
			}
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
		case ',':;
			int c = fgetc(stdin);
			tape->cells[tape->pointer] = c == EOF ? c : 0;
			break;
		case '.':
			fputc(tape->cells[tape->pointer], stdout);
			break;
		case '&':
			fputc(tape->cells[tape->pointer], stderr);
			break;
		case '#':
			/* TODO: allow -fdebug-context */
			for (usize x = 0; x < tape->tp_size; ++x) {
				/* TODO: beautify output */
				fprintf(stdout, "DEBUG: %i = %i\n", x,
						tape->cells[x]);
			}
			break;
#pragma GCC diagnostic ignored "-Wpointer-arith"
#pragma GCC diagnostic push
		case '{':
			tape->pointer -= (u8) ((void*) &tape->cells[tape->pointer]
				- memrchr(tape->cells, 0, tape->cells[tape->pointer + 1]));
			break;
		case '}':
			tape->pointer += (u8) (memchr(&tape->cells[tape->pointer], 0, tape->tp_size)
				- (void*) (&tape->cells[tape->pointer]));
			break;
#pragma GCC diagnostic pop
		case '@':
			goto cleanup;
			break;
		default:
			break;
		}
	}

cleanup:
	/* cleanup */
	if (tape->cells) free(tape->cells);
	if (tape)        free(tape);
	if (opts)        free(opts);
	for (struct Instruction *x = head->next; x->next != NULL; x = x->next)
		if (x->prev != NULL) free(x->prev);

	return 0;
}
