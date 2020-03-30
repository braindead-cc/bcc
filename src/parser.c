/* TODO: cleanup this crazy mess */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instructions.h"
#include "lbf.h"
#include "parser.h"
#include "status.h"
#include "util.h"
#include "warn.h"

/* unmatched loop stack */
struct Unmatched {
	struct Instruction *inst;
	struct Unmatched *next, *prev;
};

void
parse(struct Options *opts, char *program_data, struct Instruction *program)
{
	usize len  = strlen(program_data);
	if (opts->debug) debug("length of program data = %d", len);
	usize line = 0;
	usize column = 0;

	if (opts->verbose) status_init("parsing program");

	struct Instruction *last      = program;
	struct Unmatched   *unmatched = malloc(sizeof(struct Unmatched));

	for (usize i = 0; i < len; ++i) {
		if (opts->verbose)
			status_update("parsing program", i, (i * 100) / len);

		if (program_data[i] == '\n') {
			++line;
			/* -Wlong-lines */
			if (column > 80)
				warn(opts, line, column, W_LONG_LINES);
			column = 0;
			continue;
		} else {
			++column;
		}

		if (program_data[i] == opts->fopt_comment_char) {
			/* don't move i to newline, or that line
			 * won't get counted */
			while (program_data[i + 1] != '\n') ++i;
			continue;
		}

		usize command = 0;

		switch (program_data[i]) {
		case '+':
		case '-':
		case '[':
			if (last->command == ']')
				warn(opts, line, column, W_DEAD_CODE);
			/* fallthrough */
		case ']':
		case '<':
		case '>':
		case ',':
		case '.':
			command = program_data[i];
			break;
		case '#':
			if (opts->fopt_enable_dbg_command) {
				/* ignore shebang */
				if (program_data[i + 1] == '!') {
					if (opts->wopt_ignore_dbg)
						warn(opts, line,
							column, W_IGNORE_DBG);
					break;
				}

				command = program_data[i];
			}
			break;
		default:
			break;
		}

		/* skip non-commands */
		if (command == 0) continue;

		struct Instruction *inst = malloc(sizeof(struct Instruction));
		inst->command = command;
		inst->line    = line;
		inst->column  = column,
		inst->repeat  = 1;
		inst->prev    = last;
		last->next    = inst;

		/* match loops */
		if (program_data[i] == '[') {
			struct Unmatched *n = malloc(sizeof(struct Unmatched));
			n->inst = inst;
			n->prev = unmatched;
			n->next = NULL;
			unmatched->next = n;
			unmatched = n;
		} else if (program_data[i] == ']') {
			unmatched->inst->loop = inst;
			inst->loop = unmatched->inst;
			unmatched = unmatched->prev;
		} else {
			inst->loop = NULL;
		}

		last = inst;
	}

	last->next = NULL;

	if (opts->verbose) status_complete("parsing program");
}
