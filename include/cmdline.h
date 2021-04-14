#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#define CMD_MAX_LEN				1024

#define	CMD_LINE_ARROW			0
#define CMD_LINE_NORMAL			1

struct cmd_line {
	char buf[CMD_MAX_LEN];
	int type;
}; 

int get_pass(char *prompt, char *passwd, int max_len);
int get_user_command(char *prompt, struct cmd_line *cmd, int start);

#endif
