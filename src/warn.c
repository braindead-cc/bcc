#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "warn.h"
#include "types.h"

void
warn(struct Options *opts, usize line, usize column, usize warning)
{
	char *msg, *wflag, *hint;
	msg = wflag = hint = "";

	char *path;
	if (!strcmp("-", opts->path)) path = "<stdin>";
	else path = opts->path;

	switch (warning) {
	case W_LONG_LINES:
		if (!opts->wopt_long_lines) return;
		msg = "this line is longer than 80 columns.";
		wflag = "-Wlong-lines";
		break;
	case W_DEAD_CODE:
		if (!opts->wopt_dead_code) return;
		msg = "this statement is dead.";
		wflag = "-Wdead-code";
		break;
	case W_IGNORE_DBG:
		if (!opts->wopt_ignore_dbg) return;
		msg = "this debug statement is ignored.";
		hint = "debug statements are ignored if a '!' follows it.";
		wflag = "-Wignored-dbg";
		break;
	}

	fprintf(stderr, "%s:%d:%d: %c[32mwarning:%c[m %s [%s]\n",
			path, line, column,
			0x1B, 0x1B, msg, wflag);

	if (hint != NULL) {
		fprintf(stderr, "%s:%d:%d: %c[1mhint:%c[m %s\n",
				path, line, column,
				0x1B, 0x1B, hint);
	}
}
