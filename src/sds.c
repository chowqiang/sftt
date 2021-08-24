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
	char *tmp;

	if (str == NULL) {
		return -1;
	}	
	
	tmp = (char *)mp_malloc(g_mp, __func__, sizeof(char) * size);
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
	struct sds *str = (struct sds *)mp_malloc(g_mp, __func__, sizeof(struct sds));
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
