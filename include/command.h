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

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdbool.h>

struct user_cmd {
	char *name;
	int argc;
	char **argv;
};

struct cmd_handler {
	const char *name;
	int (*fn)(void *obj, int argc, char *argv[], bool *argv_check);
	const char *help;
	void (*usage)(void);
};

#endif

