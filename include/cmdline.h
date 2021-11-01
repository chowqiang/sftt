/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
