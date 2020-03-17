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
	printf("#include <stdio.h>\n");
	printf("#include <stdlib.h>\n");
	printf("#include <string.h>\n");
	printf("typedef %s i;", (char*) &memtype);
	printf("i *m,t,p=0;int c=0;unsigned long long s;");
	printf("main(){");
	printf("m=(i*)malloc(%d*sizeof(i));", 100000);
	printf("if(!m){puts(\"err: cannot allocate mem.\");exit(1);}");

	for (struct Instruction *c = head; c != NULL; c = c->next) {
		switch (c->command) {
		case '*':
			printf("m[p]=0;");
			break;
		case '+':
			printf("m[p]+=%d;",c->repeat);
			break;
		case '-':
			printf("m[p]-=%d;",c->repeat);
			break;
		case '[':
			printf("while (m[p]) {");
			break;
		case ']':
			printf("}");
			break;
		case '^':
			printf("p=0;");
			break;
		case '<':
			printf("p-=%d;",c->repeat);
			break;
		case '>':
			printf("p+=%d;",c->repeat);
			break;
		case ',':
			printf("c=fgetc(stdin);m[p]=c==EOF?c:0;");
			break;
		case '.':
			printf("fputc(m[p],stdout);");
			break;
		case '&':
			printf("fputc(m[p],stderr);");
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
		default:
			break;
		}
	}

	/* explicitely exit to free memory */
	printf("free(m);exit(0);\n}\n");
}
