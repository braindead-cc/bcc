/*
 * "progress bar" of a sort
 */

#include <stdio.h>
#include <string.h>

#include "status.h"
#include "terminfo.h"
#include "types.h"

#ifdef SPINNER_FANCY
static const char *spinners[10] = {
	"⠋", "⠙", "⠹", "⠸", "⠼",
	"⠴", "⠦", "⠧", "⠇", "⠏"
};
static const int splen = 9;
#else
static const char *spinners[ 4] = {
	"/", "-", "\\", "|"
};
static const int splen = 3;
#endif

static usize trmwidth;

void
status_init(char *text)
{
	/* TODO: calculate ttywidth only once
	 * currently, this is calculated every time status_init()
	 * is called, which is every time a new progress bar
	 * needs to be drawn
	 */
	trmwidth = (usize) ttywidth();
	usize spcs = trmwidth - (6 + 3 + strlen(text) + 3);

	fprintf(stderr, " %s %s...%c[%iC[  0%%]", spinners[0], text,
			0x1B, spcs);
}

void
status_update(char *text, usize ctr, usize percentage)
{
	usize spcs = trmwidth - (6 + 3 + strlen(text) + 3);

	usize spctr = percentage == STATUS_UNKNOWN ? ctr : percentage;
	fprintf(stderr, "\r %s %s...%c[%iC",
			spinners[spctr % splen],
			text, 0x1B, spcs);
	if (percentage == STATUS_UNKNOWN) {
		fputs("[  ?%]", stderr);
	} else {
		char tmp[4];
		sprintf((char*) &tmp, "%d", percentage);
		fprintf(stderr, "[%3s%%]", (char*) &tmp);
	}
}

void
status_complete(char *text)
{
	usize spcs = trmwidth - (6 + 3 + strlen(text) + 3 + 5);

	fprintf(stderr, "\r %s %s... done%c[%iC[100%%]\n",
			spinners[100 % splen],
			text, 0x1B, spcs);
}
