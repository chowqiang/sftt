#include <stdio.h>
#include <curses.h>
#include "cmdline.h"

void refill(char *prefix, char ch, int num) {
    int i = 0;
    //putchar('\r');
	printf("\r%s", prefix);
    for (i = 0; i < num; ++i) {
        putchar(ch);
   }
}

int get_pass(char *prompt, char *passwd, int max_len) {
	if (prompt) {
		printf("%s", prompt);
	}

	int i = 0;
    char ch = 0;

    initscr();
    noecho();

    for (i = 0; i < max_len;) {
		ch = getchar();
		if (ch == 13) {
			break;
		}
		if (ch == 8 || ch == 127) {
			if (i > 0) {
				refill(prompt, ' ', i);
				passwd[--i] = 0;
				refill(prompt, '*', i);
			}
			continue;
		}
		passwd[i++] = ch;
		putchar('*');
    }
    passwd[i] = 0;
    endwin();
	printf("\n");

    return i;
}
