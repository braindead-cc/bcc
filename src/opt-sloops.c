/*
 * optimize scan loops such as '[<]'
 * or [>] into a single
 * instruction ('{' or '}', respectively)
 */

#include <stddef.h>
#include <stdlib.h>

#include "instructions.h"
#include "options.h"
#include "opt-sloops.h"
#include "status.h"

void
optimize_sloops(struct Options *opts, struct Instruction *head)
{
	usize i = 0;
	if (opts->verbose)
		status_init("optimizing scan loops");
	for (struct Instruction *c = head; c != NULL; c = c->next, ++i) {
		if (opts->verbose)
			status_update("optimizing scan loops",
					i, STATUS_UNKNOWN);
		if (c->command == '['
				&& (c->next->command == '<'
					|| c->next->command == '>')
				&& c->next->next->command == ']'
		) {
			if (c->next->command == '<') c->command = '{';
			else if (c->next->command == '>') c->command = '}';

			if (c->next->next->next != NULL) {
				struct Instruction *tmp = c->next->next->next;
				free(c->next->next);
				free(c->next);
				c->next = tmp;
				tmp->prev = c;
			} else {
				free(c->next->next);
				free(c->next);
				c->next = NULL;
			}
		}
	}

	if (opts->verbose)
		status_complete("optimizing scan loops");
}
