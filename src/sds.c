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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "base.h"
#include "lock.h"
#include "mem_pool.h"
#include "sds.h"

extern struct mem_pool *g_mp;

int sds_init(struct sds *str, int size)
{
	if (str == NULL) {
		return -1;
	}	
	
	char *tmp = (char *)mp_malloc(g_mp, sizeof(char) * size);
	if (tmp == NULL) {
		str->buf = NULL;
		str->size = 0;
		str->len = 0;

		return -1;
	}
	memset(tmp, 0, size);
	str->buf = tmp;
	str->size = size;
	str->len = 0;

	return 0;
}

struct sds *sds_construct(void)
{
	struct sds *str = (struct sds *)mp_malloc(g_mp, sizeof(struct sds));
	assert(str != NULL);

	str->mutex = new(pthread_mutex);
	assert(str->mutex != NULL);

	str->buf = NULL;
	str->len = 0;
	str->size = 0;

	return str;
}

void sds_destruct(struct sds *ptr)
{
	if (ptr->buf)
		mp_free(g_mp, ptr->buf);

	ptr->buf = NULL;
	ptr->len = 0;
	ptr->size = 0;
	ptr->mutex->ops->destroy(ptr->mutex);

	mp_free(g_mp, ptr);
}

int sds_resize(struct sds *ptr, int size)
{
	if (mp_realloc(g_mp, ptr->buf, size)) {
		return -1;
	}


	if (size <= ptr->len) {
		ptr->len = size;
	}

	ptr->size = size;

	return 0;
}

int sds_add_str(struct sds *ptr, char *str)
{
	assert(ptr != NULL);

	int ret = 0;
	int len = strlen(str);

	if (len == 0)
		return 0;

	if (ptr->size == 0 && sds_init(ptr, len + 1)) {
		return -1;
	}

	int goal = 0;
	if (len >= (ptr->size - ptr->len)) {
		goal = ptr->size + len;
	}
	if (goal && sds_resize(ptr, goal))
		return -1;

	strcat(ptr->buf + ptr->len, str);
	ptr->len += len;

	return 0;
}

int sds_add_char(struct sds *ptr, char c)
{
	assert(ptr != NULL);

	int len = 1;
	if (ptr->size == 0 && sds_init(ptr, len + 1))
		return -1;

	int goal = 0;
	if (len >= (ptr->size - ptr->len))
		goal = ptr->size + len * 8;

	if (goal && sds_resize(ptr, goal))
		return -1;

	ptr->buf[ptr->len++] = c;
	ptr->buf[ptr->len] = 0;

	return 0;
}

#if 0
struct strings *create_strings(void) {
	struct strings *ss = (struct strings *)mp_malloc(g_mp, sizeof(struct strings));
	if (ss == NULL) {
		return NULL;
	}

	int init_cap = 4;
	ss->str_arr = (struct sds *)mp_malloc(g_mp, sizeof(struct sds) * init_cap);
	if (ss->str_arr == NULL) {
		mp_free(g_mp, ss);
		return NULL;
	} 
	int i = 0;
	for (i = 0; i < init_cap; ++i) {
		init_sds(&ss->str_arr[i]);
	}
	ss->cap = init_cap;
	ss->num = 0;

	return ss;
}

int add_string(struct strings *ss) {
	if (ss == NULL) {
		return -1;
	}

	if (ss->num < ss->cap) {
		ss->num++;
		return 0;
	}

	int goal = 0;
	if (ss->cap < MAX_SDS_AUTO_GROW_SIZE) {
		goal = (ss->cap != 0 ? ss->cap : 4) * 2;	
	} else {
		goal = ss->cap + 10;
	}

	struct sds *tmp = (struct sds *)realloc(ss->str_arr, sizeof(struct sds) * goal);
	if (tmp == NULL) {
		return -1;
	}
	ss->str_arr = tmp;

	int i = 0, ret = 0;
	for (i = ss->cap; i < goal; ++i) {
		ret = init_sds(&ss->str_arr[i]);
		if (ret == -1) {
			break;
		}
	}
	ss->cap = goal;
	ss->num++;

	return 0;
}

int append_char(struct strings *ss, int index, char c) {
	if (ss == NULL || ss->num <= index) {
		return -1;
	}

	return sds_append_char(&ss->str_arr[index], c);
}

int get_string_num(struct strings *ss) {
	if (ss == NULL) {
		return -1;
	}

	return ss->num;
}

const char *get_string(struct strings *ss, int index) {
	if (ss == NULL || ss->num <= index) {
		return NULL;
	}

	return (const char *)ss->str_arr[index].buf;
}

void free_strings(struct strings **ss) {
	if (ss == NULL || *ss == NULL) {
		return ;
	}
	int i = 0;
	for (i = 0; i < (*ss)->cap; ++i) {
		if ((*ss)->str_arr[i].size != 0) {
			mp_free(g_mp, (*ss)->str_arr[i].buf);
		}
	}
	mp_free(g_mp, (*ss)->str_arr);
	mp_free(g_mp, *ss);
	*ss = NULL;
}

int strings_test(void) {
	struct strings *ss = create_strings();
	if (ss == NULL) {
		return -1;
	}	
	add_string(ss);
	append_char(ss, 0, 'h');
	append_char(ss, 0, 'e');
	append_char(ss, 0, 'l');
	append_char(ss, 0, 'l');
	append_char(ss, 0, 'o');

	const char *tmp = get_string(ss, 0);
	printf("%s\n", tmp);

	return 0;
}
#endif
