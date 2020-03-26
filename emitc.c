#include <stdio.h>
#include <string.h>

#include "emitc.h"
#include "instructions.h"
#include "lbf.h"
#include "types.h"
#include "util.h"

void
emitc(struct Options *opts, struct Instruction *head)
{
	char memtype[20];
	if (opts->fopt_cell_signed) 
		strcpy(&memtype, "signed ");
	else
		strcpy(&memtype, "unsigned ");

	switch (opts->fopt_cell_size) {
	case 8:
		strcat(&memtype, "char");
		break;
	case 16:
		strcat(&memtype, "short");
		break;
	case 32:
		strcat(&memtype, "int");
		break;
	case 64:
		strcat(&memtype, "long long");
		break;
	default:
		die("lbfc: error: '%d': invalid cell size.",
				opts->fopt_cell_size);
		break;
	}

	/* includes, mov left ('<' command), etc */
	printf("#define _GNU_SOURCE\n"
		"#include <stdio.h>\n"        /* for IO */
		"#include <stdlib.h>\n"       /* for malloc(), exit() */
		"#include <string.h>\n\n"     /* for memchr(), memrchr() */
		"typedef %s i;\n\n"
		"i *m;\n"                  /* memory */
		"int c = 0;\n"                /* buffer for fgetc() */
		"unsigned long long sz, p;\n" /* tape size, pointer */
		"\nvoid ml(int repeat) {\n"
		"if ((p-repeat)>p);else p-=repeat;}\n",
		(char*) &memtype);

	/* mov right ('>' command) */
	printf("void mr(int repeat) {p+=repeat;"
		"if(p>=sz){");
	if (opts->fopt_disable_dynamic_alloc) {
		printf("p-=repeat;");
	} else {
		printf("m=realloc(m,sz*=2);"
			"if (m==NULL){"
			"perror(\"err: can't realloc mem\");"
			"exit(1);"
			"}"
			"for(unsigned long long z=sz/2;"
			"z<sz;++z,m[z]=0);");
	}
	printf("}}");

	/* read char */
	printf("void rd(void) {"
		"c = fgetc(stdin);"
		"if (c == EOF) {");
	if (opts->fopt_eof_char == EOF_NO_CHANGE)
		;
	else if (opts->fopt_eof_char == EOF_MINUS_1)
		printf("m[p]=(i)-1;");
	else if (opts->fopt_eof_char == EOF_ZERO)
		printf("m[p]=0;");
	printf("} else {"
		"m[p] = (i) c;"
		"}}");

	/* write cell */
	printf("void wr(FILE *stream) {"
		"fputc(m[p] %% 255, stream);"
		"}");

	/* main */
	printf("\nint\n"
		"main(void)\n"
		"{\n"
		"sz = p = 0;\n"
		"m = (i*) calloc(%lli, sizeof(i));\n"
		"if (!m) {\n"
		"	perror(\"err: can't allocate mem: \");\n"
		"	exit(1);\n"
		"}\n", opts->fopt_initial_tape_size);


	for (struct Instruction *c = head; c != NULL; c = c->next) {
		switch (c->command) {
		case '*':
			printf("m[p]=0;");
			break;
		case '+':
			printf("m[p]+=%i;", c->repeat);
			break;
		case '-':
			printf("m[p]-=%i;", c->repeat);
			break;
		case '[':
			printf("while(m[p]){");
			break;
		case ']':
			printf("}");
			break;
		case '^':
			printf("p=0;");
			break;
		case '<':
			printf("ml(%i);", c->repeat);
			break;
		case '>':
			printf("mr(%i);", c->repeat);
			break;
		case ',':
			printf("rd();");
			break;
		case '.':
			printf("wr(stdout);");
			break;
		case '&':
			printf("wr(stderr);");
			break;
		case '{':
			printf("p-=(i)((void*)&m[p]-memrchr(m,0,m[p]+1));");
			break;
		case '}':
			printf("p+=(i)(memchr(&m[p],0,s)-(void*)(&m[p]));");
			break;
		case '@':
			printf("free(m);exit(0);");
			break;
		case '#':
			/* deliberately not implemented */
			break;
		default:
			break;
		}
	}

	printf("free(m);exit(0);}");
}
