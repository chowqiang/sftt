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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <time.h>
#include <string.h>

#ifndef bzero
#define bzero(addr, size)	memset(addr, 0, size)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* align must be power of 2 */
#define ALIGN_FLOOR(val, align) \
		((val) & (~(align - 1)))

/* align must be power of 2 */
#define ALIGN_CEIL(val, align) \
		ALIGN_FLOOR(val + align - 1, align)

void gen_session_id(char *buf, int len);

void gen_connect_id(char *buf, int len);

int now_time_str(char *buf, int max_len);

int ts_to_str(uint64_t ts, char *buf, int max_len);

int ymd_hm_str(char *buf, int max_len);

time_t get_ts(void);

void strip(char *line);

void simplify_path(char *path);

int get_right_most_path(char *path, char *sub_path);

int get_first_word(char *buf, char *word, int len);

char *__strdup(const char *buf);

int gen_random(int min, int max);

double get_double_time(void);

#endif
