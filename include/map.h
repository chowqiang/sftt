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

#ifndef _MAP_H_
#define _MAP_H_

#include <stdbool.h>
#include <string.h>
#include "dlist.h"

struct map {
	struct dlist *list;
};

struct kv_node {
	void *key;
	void *value;
};

typedef bool (key_equal_t)(void *, void *);

static inline bool ptr_equal(void *a, void *b)
{
	return a == b;
}

static inline bool str_equal(void *a, void *b)
{
	if (a == NULL || b == NULL)
		return false;

	return strcmp((char *)a, (char *)b) == 0;
}

struct map *map_create(void);
int map_init(struct map *m);
int map_add(struct map *m, void *key, void *value);
int map_find(struct map *m, key_equal_t is_equal, void *key, void **value);
int map_remove(struct map *m, void *key, void **value);
void map_destroy(struct map *m);

void show_keys(struct map *m);
#endif
