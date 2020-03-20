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
	tape->cells = calloc(tape->tp_size, sizeof(MEMTYPE));
	if (tape->cells == NULL)
		die("lbfi: error: cannot allocate memory for tape:");
	tape->pointer = 0;
	if (opts->debug) debug("tape size = %lld", tape->tp_size);

	struct Instruction *cur = head->next;
	for (usize i = 0, depth = 0; cur != NULL; cur = cur->next, ++i) {
		switch (cur->command) {
		case '*':
			tape->cells[tape->pointer] = 0;
			break;
		case '+':
			/* TODO: custom cell size */
			tape->cells[tape->pointer] += cur->repeat;
			break;
		case '-':
			/* TODO: custom cell size */
			tape->cells[tape->pointer] -= cur->repeat;
			break;
		case '^':
			tape->pointer = 0;
			break;
		case '<':
			/* check for overflow */
			if ((tape->pointer - cur->repeat) > tape->pointer)
				; /* ignore */
			else
				tape->pointer -= cur->repeat;
			break;
		case '>':
			tape->pointer += cur->repeat;
			
			if (tape->pointer >= tape->tp_size) {
				tape->tp_size *= 2;
				if (opts->debug)
					debug("reallocating %lld for tape",
							tape->tp_size);
				tape->cells = realloc(tape->cells,
						tape->tp_size);
				if (tape->cells == NULL)
					die("lbf: error: cannot allocate memory for tape:");
				for (usize z = tape->tp_size / 2;
						z < tape->tp_size;++z,
						tape->cells[z] = 0);
			}
			break;
		case '[':
			if (tape->cells[tape->pointer] == 0) {
				depth = 1;
				while (depth > 0) {
					cur = cur->next;
					++i;
					if (cur->command == '[')
						++depth;
					else if (cur->command == ']')
						--depth;
				}
			}
			break;
		case ']':
			if (tape->cells[tape->pointer] != 0) {
				depth = 1;
				while (depth > 0) {
					cur = cur->prev;
					--i;
					if (cur->command == '[')
						--depth;
					else if (cur->command == ']')
						++depth;
				}
			}
			break;
		case ',':
			; // empty statement
			int c = fgetc(stdin);
			tape->cells[tape->pointer] = c == EOF ? 0 : c;
			break;
		case '.':
			fputc(tape->cells[tape->pointer] % 255, stdout);
			break;
		case '&':
			fputc(tape->cells[tape->pointer], stderr);
			break;
		case '#':
			/* TODO: allow -fdebug-context */
			for (usize x = 0; x < tape->tp_size; ++x) {
				fprintf(stderr, "[%i]\t\t%i\t\t0x%X\t\t0%o"
					"\t\t'%c'\n",
					x, tape->cells[x], tape->cells[x],
					tape->cells[x], tape->cells[x]);
			}
			break;
#pragma GCC diagnostic ignored "-Wpointer-arith"
#pragma GCC diagnostic push
		case '{':
			tape->pointer -= (u64) ((void*) (tape->cells + tape->pointer)
				- memrchr(tape->cells, 0, tape->pointer + 1));
			break;
		case '}':
			tape->pointer += (u64) (memchr(tape->cells + tape->pointer,
					0, tape->tp_size)
				- (void*) (tape->cells + tape->pointer));
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
	for (struct Instruction *x = head->next; x != NULL; x = x->next)
		if (x->prev != NULL) free(x->prev);

	return 0;
}
