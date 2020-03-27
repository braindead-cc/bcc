#ifndef TAPE_H
#define TAPE_H

struct Tape {
	/* number of cells */
	u64 tp_size;

	MEMTYPE *cells;

	/* cell pointer */
	u64 pointer;
};

#endif
