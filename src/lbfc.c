#include "emitc.h"
#include "instructions.h"
#include "lbf.h"
#include "lbfc.h"
#include "util.h"

int
lbfc_main(struct Options *opts, struct Instruction *head)
{
	switch (opts->backend) {
	case BOPT_C:
		emitc(opts, head);
		break;
	case BOPT_RUST:
		emit_rust(opts, head);
		break;
	case BOPT_ARM:
	case BOPT_LLVM:
	case BOPT_V:
	case BOPT_GO:
	case BOPT_QBE:
	default:
		die("lbfc: error: specified backend not implemented.");
		break;
	}

	return 0;
}
