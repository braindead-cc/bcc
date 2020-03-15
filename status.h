/*
 * implements "progress bars" of a kind
 * e.g. "- doing stuff...            [  0%]"
 */

#ifndef STATUS_H
#define STATUS_H

#include "types.h"
#define STATUS_UNKNOWN 101

void status_init(char *text);
void status_update(char *text, usize percentage);
void status_complete(char *text);

#endif
