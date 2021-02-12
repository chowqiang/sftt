#include <stdio.h>
#include <curses.h>
#include <string.h>
#include "cmdline.h"

void refill(char *prefix, char ch, int num) {
    int i = 0;
    //putchar('\r');
	printf("\r%s", prefix);
    for (i = 0; i < num; ++i) {
        putchar(ch);
	}
}

void refill_str(char *prefix, char *str, int num)
{
	int i = 0;
	printf("\r%s", prefix);
	for (i = 0; i < num; ++i) {
		putchar(str[i]);
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

void return_left(char *prefix, int num)
{
    int i = 0;
	char ch = ' ';
    //putchar('\r');
	//printf("\r%s", prefix);
	putchar('\r');
    for (i = 0; i < num; ++i) {
        putchar(ch);
	}
	putchar('\r');
	//printf("\r%s", prefix);
}

int get_user_command(char *prompt, cmd_line *cmd, int start)
{
	int i = 0;
    char ch = 0;

	if (prompt) {
		printf("%s", prompt);
	}
	printf("%s", cmd->buf);

    initscr();
    noecho();

    for (i = start; i < CMD_MAX_LEN - start - 1;) {
		ch = getchar();
		if (ch == '\033') {
			getchar();
			ch = getchar();
			switch (ch) {
			case 'A':
			case 'B':
				cmd->buf[0] = ch;
				cmd->type = CMD_LINE_ARROW;
				return_left("", strlen(prompt) + i + 3);
				endwin();
				return 1;
			case 'C':
			case 'D':
				//refill_str(prompt, cmd->buf, i + 3);
				continue;
			}
		}
		if (ch == 13) {
			break;
		}
		if (ch == 8 || ch == 127) {
			if (i > 0) {
				refill(prompt, ' ', i);
				cmd->buf[--i] = 0;
				refill_str(prompt, cmd->buf, i);
			}
			continue;
		}
		cmd->buf[i++] = ch;
		putchar(ch);
    }
    cmd->buf[i] = 0;
	cmd->type = CMD_LINE_NORMAL;
    endwin();
	printf("\n");

    return i;
}
