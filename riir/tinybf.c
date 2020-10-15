#include <stdio.h>
#include <string.h>

int main(void) {
	char dat[2048];
	char dpt = 0;
	fgets((char *)&dat,2047,stdin);

	unsigned char mem[30000];
	memset(&mem, 0x0, 30000-1);
	unsigned char ptr = 0;
	while (dat[dpt]) {
		if (dat[dpt] == '<') {--ptr; }
		else if (dat[dpt] == '>') { ++ptr; }
		else if (dat[dpt] == '+'){ ++mem[ptr] ;}
		else if (dat[dpt] == '-'){ --mem[ptr] ;}
		else if (dat[dpt] == '.'){ putchar(mem[ptr]) ;}
		else if (dat[dpt] == ','){ mem[ptr]=getchar() ;}
		else if (dat[dpt] == '['){
			if (mem[ptr] == 0) {
				int st = 1;
				while(dat[++dpt]&&st!=0) {
					if (dat[dpt] == '[') {
						++st;
					} else if (dat[dpt] == ']') {
						--st;
					}
				}
			}
		}
		else if (dat[dpt] == ']'){
			if (mem[ptr] != 0) {
				int st = 1;
				while(dat[--dpt]&&st!=0) {
					if (dat[dpt]  == ']') {
						--st;
					} else if (dat[dpt] == '[') {
						++st;
					}
				}
			}
		}
		++dpt;
	}

	printf("debug:\n");
	for (int i = 0; i < 50; ++i) {
		printf("index: %d\tvalue: %hhd\tchar: '%c'\n", i, mem[i], mem[i]);
	}
}
