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

	/* print boilerplate (deliberately obfuscated) */
	printf("#define _GNU_SOURCE\n");
	printf("#include <stdio.h>\n");
	printf("#include <stdlib.h>\n");
	printf("#include <string.h>\n\n");
	printf("typedef %s i;\n\n", (char*) &memtype);
	printf("i *m, t;\nint c = 0;\nunsigned long long s, p = 0;\n");
	printf("int\nmain(void)\n{\n");
	printf("m = (i*) malloc(%d * sizeof(i));\n", 100000);
	printf("if (!m) {\nputs(\"err: cannot allocate mem.\");\nexit(1);\n}\n");

	for (struct Instruction *c = head; c != NULL; c = c->next) {
		switch (c->command) {
		case '*':
			printf("m[p] = 0;\n");
			break;
		case '+':
			printf("m[p] += %i;\n",c->repeat);
			break;
		case '-':
			printf("m[p] -= %i;\n",c->repeat);
			break;
		case '[':
			printf("while (m[p]) {\n");
			break;
		case ']':
			printf("}\n");
			break;
		case '^':
			printf("p = 0;\n");
			break;
		case '<':
			/* TODO: ptr wrapping */
			printf("p -= %i;\n",c->repeat);
			break;
		case '>':
			/* TODO: dynamic allocation */
			printf("p += %i;\n",c->repeat);
			break;
		case ',':
			printf("c=fgetc(stdin);m[p]=c==EOF?c:0;\n");
			break;
		case '.':
			printf("fputc(m[p],stdout);\n");
			break;
		case '&':
			printf("fputc(m[p],stderr);\n");
			break;
		case '{':
			printf("p-=(i)((void*)&m[p]-memrchr(m,0,m[p]+1));\n");
			break;
		case '}':
			printf("p+=(i)(memchr(&m[p],0,s)-(void*)(&m[p]));\n");
			break;
		case '@':
			printf("free(m);exit(0);\n");
			break;
		case '#':
			/* deliberately not implemented */
		default:
			break;
		}
	}

	printf("free(m);\nexit(0);\n}\n");
}
