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

#ifndef _OPTION_H_
#define _OPTION_H_

#define NO_ARG					0x0000
#define HAS_ARG					0x0001
#define OPT_ARG					0x0010

enum option_index {
	USER,
	HOST,
	PORT,
	PASSWORD,
	START,
	RESTART,
	STOP,
	STATUS,
	DB,
	ADDUSER,
	DAEMON,
	STORE,
	UNKNOWN_OPT
};

struct sftt_option {
	const char *name;
	enum option_index index;
	int flags;
};

char *next_arg(char *arg, char **param, char **value);

const struct sftt_option *lookup_opt(int argc, char **argv, char **optarg,
	int *optind, const struct sftt_option *sftt_opts);

#endif

