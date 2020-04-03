#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "util.h"

usize
die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	exit(1);

	/* return dummy value so that die() can
	 * be used with a boolean expression
	 * e.g.: is_ok || die("message")
	 */
	return 0;
}

usize
debug(const char *fmt, ...)
{
	fprintf(stderr, "bcc: debug: ");

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fputc('\n', stderr);

	return 0;
}
