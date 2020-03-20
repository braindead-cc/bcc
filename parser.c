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

void
parse(struct Options *opts, char *program_data, struct Instruction *program)
{
	usize len  = strlen(program_data);
	if (opts->debug) debug("length of program data = %d", len);
	usize line = 0;
	usize column = 0;

	if (opts->verbose) status_init("parsing program");

	struct Instruction *last = program;
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

		usize command = 0;

		switch (program_data[i]) {
		case '+':
		case '-':
		case '[':
			if (last->command == ']' || last->command == '*')
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
		case '*':
			if (last->command == ']' || last->command == '*') {
				if (opts->fopt_enable_lbf_std)
					warn(opts, line, column, W_DEAD_CODE);
			}
			/* fallthrough */
		case '^':
		case '&':
		case '{':
		case '}':
		case '@':
			if (opts->fopt_enable_lbf_std)
				command = program_data[i];
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

		last = inst;
	}

	last->next = NULL;

	if (opts->verbose) status_complete("parsing program");
}
