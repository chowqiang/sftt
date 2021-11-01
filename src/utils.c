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

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "mem_pool.h"
#include "utils.h"

extern struct mem_pool *g_mp;

int gen_int(int begin, int end)
{
	time_t t;

	assert(end > begin);
	t = time(NULL);
	srand((unsigned)(t));

	return rand() % (end - begin) + begin;
}

char gen_char(char begin, char end)
{
	time_t t;

	assert(end > begin);
	t = time(NULL);
	srand((unsigned)(t));

	return rand() % (end - begin) + begin;
}

int gen_id(void)
{
	int MAX_ID = (1 << 30);
	int MIN_ID = ( 1 << 29);

	return gen_int(MIN_ID, MAX_ID);
}

void simple_session_id(char *buf, int len)
{
	int i;
	time_t t;

	t = time(NULL);
	srand((unsigned)(t));

	for (i = 0; i < len - 1; ++i) {
		if (i % 2) {
			//buf[i] = gen_char('a', 'z');
			buf[i] = rand() % ('z' - 'a') + 'a';
		} else {
			//buf[i] = gen_char('0', '9');
			buf[i] = rand() % ('9' - '0') + '0';
		}
	}

	buf[len] = 0;
}

void gen_session_id(char *buf, int len)
{
	const int SESSION_ID_PART1_LEN = 3;
	const int SESSION_ID_PART2_LEN = 3;

	char part1[SESSION_ID_PART1_LEN + 1];
	char part2[SESSION_ID_PART2_LEN + 1];

	static time_t last_ts = 0;
	static int last_id = 0;

	int i, id;
	time_t t, ts;

	t = time(NULL);
	srand((unsigned)(t));

	ts = get_ts();
	if (ts != last_ts)
		last_id = -1;
	id = last_id + 1;

	bzero(part1, sizeof(part1));
	part1[0] = 's';
	for (i = 1; i < SESSION_ID_PART1_LEN; ++i)
		part1[i] = rand() % ('z' - 'a') + 'a';

	bzero(part2, sizeof(part2));
	for (i = 0; i < SESSION_ID_PART2_LEN; ++i)
		part2[i] = rand() % ('z' - 'a') + 'a';

	snprintf(buf, len, "%s%lu%s%04d", part1, ts, part2, id);

	buf[len] = 0;

	last_ts = ts;
	last_id = id;
}

void gen_connect_id(char *buf, int len)
{
	static time_t last_ts = 0;
	static int last_id = 0;

	int id;
	time_t ts;

	ts = get_ts();
	if (ts != last_ts)
		last_id = -1;
	id = last_id + 1;

	snprintf(buf, len, "%lu%06d", ts, id);

	buf[len] = 0;

	last_ts = ts;
	last_id = id;
}

bool is_int(char *buf, int *num)
{
	if (!(buf[0] == '-' || buf[0] == '+' || isdigit(buf[0]))) {
		return false;
	}

	if (strlen(buf) > 11) {
		return false;
	}

	int i;
	for (i = 1; buf[i]; ++i) {
		if (!isdigit(buf[i])) {
			return false;
		}
	}

	return atol(buf) == atoi(buf);
}

int ymd_hm_str(char *buf, int max_len)
{
	time_t ts = time(NULL);
	const char *format = "%Y%m%d%H";
    struct tm lt;
	int ret = 0;

    (void) localtime_r(&ts, &lt);
    if ((ret = strftime(buf, max_len, format, &lt)) == 0) {
		return sprintf(buf, "unknown");
	}

	return ret;
}

int now_time_str(char *buf, int max_len)
{
	time_t ts = time(NULL);
	return ts_to_str((uint64_t)ts, buf, max_len);
}

int ts_to_str(uint64_t ts, char *buf, int max_len)
{
	time_t t = (time_t)ts;
    const char *format = "%Y-%m-%d %H:%M:%S";
    struct tm lt;
	int ret = 0;

    (void) localtime_r(&t, &lt);
    if ((ret = strftime(buf, max_len, format, &lt)) == 0) {
		return sprintf(buf, "unknown");
	}

	return ret;
}

time_t get_ts(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);

	return (time_t)t.tv_sec;
}

void strip(char *line)
{
	int len = strlen(line);

	while (len > 0 && line[len - 1] == ' ') {
		line[--len] = 0;
	}

	if (len < 1) {
		return ;
	}

	int i = 0;
	while (i < len && line[i] == ' ') {
		++i;
	}
	if (i >= len) {
		line[0] = 0;
		return ;
	}

	int j = 0;
	for (j = 0; i <= len; ++i, ++j) {
		line[j] = line[i];
	}
}

void simplify_path(char *path)
{
	int i = 0, j = 0, len = 0;
	char *tmp = NULL;
	bool slash = false;

	len = strlen(path);
	if (len == 0)
		return ;

	tmp = (char *)mp_malloc(g_mp, __func__, sizeof(char) * (len + 1));
	assert(tmp);

	i = len - 1;
	while (i > 0 && path[i] == '/')
		path[i--] = 0;

	i = 0;
	while (path[i]) {
		if (path[i] == '/' && slash == true) {
			++i;
			continue;
		}
		tmp[j++] = path[i];
		slash = path[i] == '/';
		++i;
	}

	tmp[j] = 0;
	strcpy(path, tmp);

	mp_free(g_mp, tmp);
}

int get_right_most_path(char *path, char *sub_path)
{
	int len = 0, idx = 0;
	char *pos = NULL;

	if (path == NULL || sub_path == NULL)
		return -1;

	simplify_path(path);

	if ((len = strlen(path)) == 0) {
		sub_path[0] = 0;
		return 0;
	}

	if ((pos = rindex(path, '/')) == NULL) {
		strcpy(sub_path, path);
		return 0;
	}

	strcpy(sub_path, pos + 1);

	return 0;
}

int get_first_word(char *buf, char *word, int len)
{
	char *pos = NULL;
	int blen = 0;

	if (buf == NULL || word == NULL || len <= 0)
		return -1;

	blen = strlen(buf);
	if (blen == 0)
		return -1;

	if ((pos = index(buf, ' ')) == NULL) {
		if (blen < len)
			return -1;

		strcpy(word, buf);

		return 0;
	}

	if ((pos - buf) > len)
		return -1;

	strncpy(word, buf, buf - pos);

	return 0;
}

char *__strdup(const char *buf)
{
	int len;
	char *tmp;

	if (buf == NULL || (len = strlen(buf)) == 0)
		return NULL;

	tmp = mp_malloc(g_mp, __func__, len + 1);
	if (tmp == NULL)
		return tmp;

	strcpy(tmp, buf);

	return tmp;
}

int gen_random(int min, int max)
{
	return gen_int(min, max);
}
