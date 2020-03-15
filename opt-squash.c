/*
 * "squash" command together
 * e.g. +++++++++ => 9+
 */

#include <stddef.h>
#include <stdlib.h>

#include "instructions.h"
#include "opt-squash.h"

void
optimize_squash(struct Instruction *head)
{
	for (struct Instruction *c = head; c != NULL; c = c->next) {
		if (c->next != NULL && c->command == c->next->command) {
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
				++repeated;
			}
			tmp->repeat += repeated;
		}
	}
}
