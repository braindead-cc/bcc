#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "instructions.h"
#include "options.h"
#include "debugger.h"
#include "tape.h"
#include "util.h"

static void setborder(WINDOW *w);
static void update_code_w(struct Instruction *cur, WINDOW *w);
static void update_mem_w(struct Tape *tape, WINDOW *w);

void
debugger_main(struct Options *opts, struct Instruction *head)
{
	bool execute = TRUE;

	/* init ncurses */
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, execute);
	curs_set(FALSE);

	/* exit if terminal is too small */
	if (LINES < 24 || COLS < 80) {
		die("bccd: error: terminal is too small.\n"
			"bccd: hint: minimum terminal size for "
			"debugger is 80x24.");
	}

	/* initialize brainfuck */
	struct Tape *tape = malloc(1 * sizeof(struct Tape));
	if (tape == NULL)
		die("bccd: error: cannot allocate memory for tape:");
	tape->tp_size = opts->fopt_initial_tape_size;
	tape->cells = calloc(tape->tp_size, sizeof(MEMTYPE));
	if (tape->cells == NULL)
		die("bccd: error: cannot allocate memory for tape:");
	tape->pointer = 0;

	/* delay */
	struct timespec delay = {
		0, 100000000,
	};

	/* windows */
	WINDOW *code_w = newwin(5, COLS, 0, 0); /* code area */
	WINDOW *mem_w  = newwin(5, COLS, 6, 0); /* memory */

	/* set borders */
	setborder(mem_w);
	setborder(code_w);

	int ch;
	for (
		struct Instruction *cur = head->next;
		cur != NULL;
		cur = cur->next
	) {
		/* handle input events */
		if ((ch = getch()) == ERR) {
			;
		} else {
			switch (ch) {
			case '+': /* increase speed */
				delay.tv_nsec -= 500000000;
				break;
			case '-': /* decrease speed */
				delay.tv_nsec += 500000000;
				break;
			case ' ': /* toggle execution */
				execute = !execute;
				nodelay(stdscr, execute);
			default:
				break;
			}
		}

		update_code_w(cur, code_w);
		wrefresh(code_w);
		update_mem_w(tape, mem_w);
		wrefresh(mem_w);

		/* execute brainfsck */
		switch (cur->command) {
		case '*':
			tape->cells[tape->pointer] = 0;
			break;
		case '+':
			tape->cells[tape->pointer] += cur->repeat;
			break;
		case '-':
			tape->cells[tape->pointer] -= cur->repeat;
			break;
		case '<':
			/* check for overflow */
			if ((tape->pointer - cur->repeat) > tape->pointer)
				; /* ignore */
			else
				tape->pointer -= cur->repeat;
			break;
		case '>':
			tape->pointer += cur->repeat;

			if (tape->pointer >= tape->tp_size) {
				if (opts->fopt_disable_dynamic_alloc) {
					tape->pointer -= cur->repeat;
					continue;
				}

				tape->tp_size *= 2;
				if (opts->debug)
					debug("reallocating %lld for tape",
							tape->tp_size);
				tape->cells = realloc(tape->cells,
						tape->tp_size);
				if (tape->cells == NULL)
					die("bccd: error: cannot allocate"
							"memory for tape:");
				for (usize z = tape->tp_size / 2;
						z < tape->tp_size;++z,
						tape->cells[z] = 0);
			}
			break;
		case '[':
			if (tape->cells[tape->pointer] == 0) {
				cur = cur->loop;
			}

			break;
		case ']':
			if (tape->cells[tape->pointer] != 0) {
				cur = cur->loop;
			}

			break;
		case '.':
		case ',':
			/* TODO: implement */
			break;
		case '#':
			/* deliberately not implemented */
			break;
#pragma GCC diagnostic ignored "-Wpointer-arith"
#pragma GCC diagnostic push
		case '{':
			tape->pointer -= (u64) ((void*) (tape->cells + tape->pointer)
				- memrchr(tape->cells, 0, tape->pointer + 1));
			break;
		case '}':
			tape->pointer += (u64) (memchr(tape->cells + tape->pointer,
					0, tape->tp_size)
				- (void*) (tape->cells + tape->pointer));
			break;
#pragma GCC diagnostic pop
		default:
			break;

		}


		nanosleep(&delay, NULL);
	}

	/* reset term */
	nodelay(stdscr, FALSE);
	getch();
	delwin(code_w);
	endwin();

	free(opts);
	free(tape->cells);
	free(tape);

	for (struct Instruction *x = head->next; x != NULL; x = x->next)
		if (x->prev != NULL) free(x->prev);
}

static void
setborder(WINDOW *w)
{
	wborder(
		w,
		ACS_VLINE, ACS_VLINE,
		ACS_HLINE, ACS_HLINE,
		ACS_ULCORNER, ACS_URCORNER,
		ACS_LLCORNER, ACS_LRCORNER
	);
}

static void
update_code_w(struct Instruction *cur, WINDOW *w)
{
	/* write label */
	mvwprintw(w, 0, 3, " code ");

	/* write code */
	usize code_width = COLS - 2; /* length of space on screen for code */
	char code[code_width]; /* code buffer */

	/* set initial value of code buffer */
	for (usize i = 0; i < code_width; code[i] = ' ', ++i);

	/* write code into buffer left of ptr */
	struct Instruction *j = cur;
	for (usize i = code_width/2; i != 0; j = j->prev, --i) {
		if (j->prev == NULL) /* this is the head node */
			break;

		code[i] = j->command;
	}
	
	/* write code into buffer right of ptr */
	j = cur;
	for (usize i = code_width/2; i < code_width; j = j->next, ++i) {
		if (j == NULL) /* end node */
			break;
		code[i] = j->command;
	}

	code[code_width] = '\0'; /* nul terminator */

	/* draw code onto screen */
	mvwprintw(w, 2, 1, "%s", code);

	/* draw cursor
	 * TODO: separate this into init_code_w */
	mvwaddch(w, 3, (code_width/2), (const chtype) '^');
}

static void
update_mem_w(struct Tape *tape, WINDOW *w)
{
	/* label */
	mvwprintw(w, 0, 3, " memory (0x%X) ", tape->pointer);

	usize sz = COLS - 2;
	char buf[sz];

	/* set initial value of buf */
	for (usize i = 0; i < sz; buf[i] = ' ', ++i);

	usize ptr = tape->pointer - ((sz / 2) / 4);

	/* fill buffer left of ptr */
	for (usize i = 0; i < sz; ++ptr, i += 4) {
		if (ptr > tape->tp_size) {
			buf[i] = buf[i + 1] = buf[i + 2]
				= buf[i + 3] = ' ';
			continue;
		}

		char tmp[5];
		tmp[0] = '\0';
		sprintf((char*) &tmp, "%03i ", tape->cells[ptr]);
		buf[i]   = tmp[0];
		buf[i+1] = tmp[1];
		buf[i+2] = tmp[2];
		buf[i+3] = tmp[3];
	}

	buf[sz] = '\0'; /* nul terminator */

	/* draw code onto screen */
	mvwprintw(w, 2, 1, "%s", buf);

	/* draw cursor
	 * TODO: separate this into init_mem_w */
	mvwprintw(w, 3, (sz / 2), "%s", "^^^");
}
