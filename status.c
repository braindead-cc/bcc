/*
 * "progress bar" of a sort
 */

#ifdef SPINNER_FANCY
static const char *spinners[10] = {
	"⠋", "⠙", "⠹", "⠸", "⠼",
	"⠴", "⠦", "⠧", "⠇", "⠏"
};
#else
static const char *spinners[ 4] = {
	"|", "/", "-", "\\"
};
#endif

static u16 trmwidth;

void
status_init(char *text)
{
	fprintf(stderr, " %c %s...", spinners[0], text);

	trmwidth = ttywidth();
	usize spcs = trmwidth - (6 + 3 + strlen(text) + 3);
	for (usize s = 0; s < spcs; ++s) fputc(' ', stderr);

	fprintf(stderr, "[  0%%]");
}

void
status_update(char *text, usize percentage)
{
	fprintf(stderr, "\r %c %s...",
			spinners[percentage % (sizeof(spinners) - 1)],
			text);
	usize spcs = trmwidth - (6 + 3 + strlen(text) + 3);
	for (usize s = 0; s < spcs; ++s) fputc(' ', stderr);

	if (percentage == STATUS_UNKNOWN)
		fputs("[  ?%]", stderr);
	else
		fprintf(stderr, "[%3s%%]", itoa(percentage));
}

void
status_complete(char *text)
{
	fprintf(stderr, "\r %c %s... done",
			spinners[100 % (sizeof(spinners) - 1)],
			text);
	usize spcs = trmwidth - (6 + 3 + strlen(text) + 3 + 5);
	for (usize s = 0; s < spcs; ++s) fputc(' ', stderr);

	fputs("[100%]", stderr);
}
