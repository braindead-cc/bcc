#include <stdio.h>
#include <string.h>

#include "emit_rust.h"
#include "instructions.h"
#include "options.h"
#include "types.h"
#include "util.h"

void
emit_rust(struct Options *opts, struct Instruction *head)
{
	char memtype[9];
	if (opts->fopt_cell_signed) 
		strcpy((char*) &memtype, "");
	else
		strcpy((char*) &memtype, "u");

	switch (opts->fopt_cell_size) {
	case 8:
		strcat((char*) &memtype, "int8");
		break;
	case 16:
		strcat((char*) &memtype, "int16");
		break;
	case 32:
		strcat((char*) &memtype, "int32");
		break;
	case 64:
		strcat((char*) &memtype, "int64");
		break;
	default:
		/* TODO: mov to arg parsing area */
		die("bcc: c: error: '%d': invalid cell size.",
				opts->fopt_cell_size);
		break;
	}

	/* main */
	printf("package main\n"
		"import \"fmt\"\n\n");

	/* type */
	printf("type MEMTYPE %s;\n\n", (char*) &memtype);

	/* memory, pointer */
	printf("m := make([]MEMTYPE, %lli)\n\n", opts->fopt_initial_tape-Isze);

	/* mov right ('>' command) */
	printf("fn mr(st: &mut State, repeat: usize) {\n"
		"    st.p = usize::wrapping_add(st.p, repeat);\n"
		"    let capacity = st.m.capacity();\n"
		"    if st.p >= capacity {\n");
	if (!opts->fopt_disable_dynamic_alloc) {
		printf("    st.m.reserve(capacity);\n");
	}
	printf("    }\n"
		"}\n\n");

	/* read char */
	printf("fn rd(st: &mut State) {\n"
		"    let bite = io::stdin().bytes().next();\n"
		"    if !bite.is_some() {\n");
	if (opts->fopt_eof_char == EOF_NO_CHANGE)
		;
	else if (opts->fopt_eof_char == EOF_MINUS_1)
		printf("        st.m[st.p] = -1 as MEMTYPE;\n");
	else if (opts->fopt_eof_char == EOF_ZERO)
		printf("        st.m[st.p] = 0 as MEMTYPE;\n");
	printf("    }\n"
		"    else {\n"
		"        st.m[st.p] = bite.unwrap().unwrap() as MEMTYPE\n"
		"    }\n"
		"}\n\n");

	/* print a character */
	printf("fn wr(st: &mut State) {\n"
		"   print!(\"{}\", ((st.m[st.p] %% 255) as u8) as char);\n"
		"   io::stdout().flush().unwrap();\n"
		"}\n");

	/* main */
	printf("fn main() {\n"
		"    let mut st = State {\n"
		"        p: 0,\n"
		"        m: vec![0; %lli],\n"
		"    };\n\n"
		"    // ------ BEGIN GENERATED CONTENT ------\n\n",
		opts->fopt_initial_tape_size);


	for (struct Instruction *c = head; c != NULL; c = c->next) {
		printf("\n    ");

		switch (c->command) {
		case '*':
			printf("st.m[st.p] = 0;");
			break;
		case '+':
			printf("st.m[st.p] = %s::wrapping_add("
				"st.m[st.p], %i );",
				(char*) &memtype, c->repeat);
			break;
		case '-':
			printf("st.m[st.p] = %s::wrapping_sub("
				"st.m[st.p], %i );",
				(char*) &memtype, c->repeat);
			break;
		case '[':
			printf("while st.m[st.p]!=0 {");
			break;
		case ']':
			printf("}");
			break;
		case '<':
			printf("st.p = usize::saturating_sub(st.p, %i);",
				c->repeat);
			break;
		case '>':
			printf("mr(&mut st, %i);", c->repeat);
			break;
		case ',':
			printf("rd(&mut st);");
			break;
		case '.':
			printf("wr(&mut st);");
			break;
		case '{':
			/* TODO: optimize with memrchr */
			printf("while st.m[st.p]!=0 {"
				"st.p=%s::saturating_sub(st.p,1_u64);}",
				(char*) &memtype);
			break;
		case '}':
			/* TODO: optimize with memchr */
			printf("while st.m[st.p]!=0 {mr(&mut st, 1_64);}");
			break;
		case '#':
			/* deliberately not implemented */
			break;
		default:
			break;
		}
	}

	printf("\n}\n");
}
