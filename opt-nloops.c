/*
 * optimize nullifying loops
 * such as '[-]' into a single
 * cell-zeroing instruction: *
 */

#include <stddef.h>
#include <stdlib.h>

#include "instructions.h"
#include "lbf.h"
#include "opt-nloops.h"
#include "status.h"

void
optimize_nloops(struct Options *opts, struct Instruction *head)
{
	usize i = 0;
	if (opts->verbose)
		status_init("optimizing nullification loops");
	for (struct Instruction *c = head; c != NULL; c = c->next, ++i) {
		if (opts->verbose)
			status_update("optimizing nullification loops",
					i, STATUS_UNKNOWN);
		if (c->command == '['
				&& (c->next->command == '-'
					|| c->next->command == '+')
				&& c->next->next->command == ']'
		) {
			c->command = '*';
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
		status_complete("optimizing nullification loops");
}
