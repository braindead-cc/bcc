/*
 * "squash" command together
 * e.g. +++++++++ => 9+
 */

#include <stddef.h>
#include <stdlib.h>

#include "instructions.h"
#include "options.h"
#include "opt-squash.h"
#include "status.h"

void
optimize_squash(struct Options *opts, struct Instruction *head)
{
	usize i = 0;
	if (opts->verbose)
		status_init("optimizing multiple commands");
	for (struct Instruction *c = head; c != NULL; c = c->next, ++i) {
		if (opts->verbose)
			status_update("optimizing multiple commands",
					i, STATUS_UNKNOWN);
		if (c->next != NULL
				&& c->command == c->next->command
				&& (c->command == '+' ||
					c->command == '-' ||
					c->command == '<' ||
					c->command == '>'
		)) {
			struct Instruction *tmp = c;
			c = c->next;
			usize repeated = 0;
			while (tmp->command == c->command) {
				if (c->next != NULL)
					c = c->next;
				else
					break;
				free(c->prev);
				tmp->next = c;
				c->prev   = tmp;
				++repeated;
			}
			tmp->repeat += repeated;
		}
	}

	if (opts->verbose)
		status_complete("optimize multiple commands");
}
