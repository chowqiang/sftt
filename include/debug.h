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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdbool.h>
#include <stdio.h>
#include "autoconf.h"
#include "utils.h"

extern int default_debug_level;

enum debug_level {
	DEBUG_DEBUG = 1,
	DEBUG_INFO,
	DEBUG_WARN,
	DEBUG_ERROR,
};

void set_client_debug_level(int verbose);
void set_server_debug_level(int verbose);

static inline char *get_debug_level_desc(int print_level)
{
	switch (print_level) {
	case DEBUG_DEBUG:
		return "DEBUG";
	case DEBUG_INFO:
		return "INFO";
	case DEBUG_WARN:
		return "WARN";
	case DEBUG_ERROR:
		return "ERROR";
	default:
		return "*";
	}
}

#define DEBUG_ASSERT(Expression, fmt, ...)		\
	do {										\
		if (!(Expression)) {					\
			char __now[32];			\
			now_time_str(__now, 31);		\
			printf("[%s][%s](%s|%d) assert(%s) failed! "fmt,			\
				get_debug_level_desc(DEBUG_INFO),		\
				__now,			\
				__func__,						\
				__LINE__,						\
				#Expression,					\
				##__VA_ARGS__);					\
			return -1;							\
		}										\
	} while (false)

static inline void print_nch(char ch, int num) {
	int __i = 0;
	for (; __i < num; __i++) {
		putchar(ch);
	}
	putchar('\n');
}

#define print_split_line				\
	do {								\
		print_nch('=', 80);				\
	} while (false)

#define debug_print(print_level, fmt, ...)		\
	if (print_level >= default_debug_level) {		\
		char __now[32];			\
		now_time_str(__now, 31);		\
		printf("[%s][%s](%s|%d) "fmt,				\
			get_debug_level_desc(print_level),		\
			__now,			\
			__func__,							\
			__LINE__,							\
			##__VA_ARGS__);						\
	}

#if defined(CONFIG_DEBUG)
	#define DEBUG(Expression)	\
		do {					\
			debug_print Expression \
		} while (false)
#else
	#define DEBUG(Expression)
#endif

#define DEBUG_POINT	DEBUG((DEBUG_INFO, "\n"))

#endif
