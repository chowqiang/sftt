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

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "map.h"
#include "mem_pool.h"

extern struct mem_pool *g_mp;

/*
 * Init the map struct
 * @m: Map struct pointer
 *
 * Return: 0 if init successfully; < 0 if an error occurred
 */
int map_init(struct map *m)
{
	DBUG_ENTER(__func__);

	if (m == NULL) {
		DBUG_RETURN(-1);
	}

	m->list = dlist_create(FREE_MODE_MP_FREE);
	if (m->list == NULL) {
		mp_free(g_mp, m);
		DBUG_RETURN(-1);
	}

	DBUG_RETURN(0);
}

struct map *map_create(void)
{
	DBUG_ENTER(__func__);

	struct map *m = (struct map *)mp_malloc(g_mp, __func__,
			sizeof(struct map));
	if (m == NULL) {
		DBUG_RETURN(NULL);
	}
	m->list = dlist_create(FREE_MODE_MP_FREE);
	if (m->list == NULL) {
		mp_free(g_mp, m);
		DBUG_RETURN(NULL);
	}

	DBUG_RETURN(m);
}

int map_add(struct map *m, void *key, void *value)
{
	DBUG_ENTER(__func__);

	if (m == NULL) {
		printf("map object cannot be NULL\n");
		DBUG_RETURN(-1);
	}

	struct kv_node *data = (struct kv_node *)mp_malloc(g_mp,
			__func__, sizeof(struct kv_node));
	if (data == NULL) {
		printf("cannot alloc kv_node object\n");
		DBUG_RETURN(-1);
	}

	data->key = key;
	data->value = value;

	struct kv_node *kvn = NULL;
	struct dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (struct kv_node *)ln->data;
		//printf("%s:%d, kvn->key=%s\n", kvn->key);
		if (kvn->key == data->key) {
			kvn->value = data->value;
			mp_free(g_mp, data);
			DBUG_RETURN(0);
		}
	}

	DBUG_RETURN(dlist_append(m->list, (void *)data));
}

int map_find(struct map *m, key_equal_t is_equal, void *key, void **value)
{
	DBUG_ENTER(__func__);

	if (m == NULL) {
		printf("map object cannot be NULL\n");
		DBUG_RETURN(-1);
	}
	struct kv_node *kvn = NULL;
	struct dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (struct kv_node *)ln->data;
		//printf("map_key: %s\n", kvn->key);
		if (is_equal(kvn->key, key)) {
			if (value) {
				*value = kvn->value;
			}
			DBUG_RETURN(0);
		}
	}

	//printf("cannot find elem!\n");
	DBUG_RETURN(-1);
}

int map_remove(struct map *m, void *key, void **value)
{
	DBUG_ENTER(__func__);

	if (m == NULL) {
		DBUG_RETURN(-1);
	}
	struct kv_node *kvn = NULL;
	struct dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (struct kv_node *)ln->data;
		if (kvn->key == key) {
			dlist_remove(m->list, ln, value, 1);
			DBUG_RETURN(0);
		}
	}

	DBUG_RETURN(-1);
}

void map_destroy(struct map *m)
{
	DBUG_ENTER(__func__);

	if (m == NULL || m->list == NULL) {
		DBUG_VOID_RETURN;
	}

	dlist_destroy(m->list);
	mp_free(g_mp, m);

	DBUG_VOID_RETURN;
}

void show_keys(struct map *m)
{
	DBUG_ENTER(__func__);

	if (m == NULL)
		DBUG_VOID_RETURN;

	struct kv_node *kvn = NULL;
	struct dlist_node *ln = NULL;
	printf("map keys:");
	dlist_for_each(m->list, ln) {
		kvn = (struct kv_node *)ln->data;
		printf(" %s", (char *)kvn->key);
	}
	printf("\n");

	DBUG_VOID_RETURN;
}
