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
#include <stdio.h>
#include <stdlib.h>
#include "dlist.h"
#include "mem_pool.h"
#include "show.h"

extern struct mem_pool *g_mp;

/*
 * Create a node of dlist
 * @data: the data for the node
 *
 * Return: dlist node pointer
 */
struct dlist_node *dlist_node_create(void *data)
{
	struct dlist_node *node = (struct dlist_node *)mp_malloc(g_mp,
			__func__, sizeof(struct dlist_node));
	if (node == NULL) {
		return NULL;
	}

	node->data = data;
	node->prev = node->next = NULL;
	
	return node;
}

struct dlist *dlist_create(enum free_mode mode)
{
	struct dlist *list = (struct dlist *)mp_malloc(g_mp,
			__func__, sizeof(struct dlist));
	dlist_init(list, mode);

	return list;
}

void dlist_init(struct dlist *list, enum free_mode mode)
{
	if (list == NULL) {
		return ;
	}

	list->size = 0;
	list->free_mode = mode;
	list->show = NULL;
	list->head = NULL;
	list->tail = NULL;
}

void dlist_destroy(struct dlist *list)
{
	if (list == NULL) {
		return ;
	}

	struct dlist_node *p = list->head;
	struct dlist_node *q = NULL;
	while (p) {
		switch (list->free_mode) {
		case FREE_MODE_NOTHING:
			break;
		case FREE_MODE_MP_FREE:
			mp_free(g_mp, p->data);
			break;
		case FREE_MODE_FREE:
			free(p->data);
			break;
		}

		q = p->next;
		mp_free(g_mp, p);
		p = q;
	}
}

int dlist_ins_next(struct dlist *list, struct dlist_node *elem, void *data)
{
	if (list == NULL || elem == NULL) {
		return -1;
	}

	struct dlist_node *node = dlist_node_create(data);
	if (node == NULL) {
		return -1;
	}

	struct dlist_node *next = elem->next;
	node->next = next;
	if (next) {
		next->prev = node;
	}
	node->prev = elem;
	elem->next = node;
	
	list->size += 1;

	if (list->tail == elem) {
		list->tail = node;
	}

	return list->size;
}

int dlist_ins_prev(struct dlist *list, struct dlist_node *elem, void *data)
{
	if (list == NULL || elem == NULL) {
		return -1;
	}
	
	struct dlist_node *node = dlist_node_create(data);
	if (node == NULL) {
		return -1;
	}

	struct dlist_node *prev = elem->prev;
	node->prev = prev;
	if (prev) {
		prev->next = node;
	}
	node->next = elem;
	elem->prev = node;

	list->size += 1;

	if (list->head == elem) {
		list->head = node;
	}
	
	return list->size;
}

int dlist_prepend(struct dlist *list, void *data)
{
	if (list == NULL) {
		return -1;
	}
	if (list->head == NULL) {
		assert(list->head == NULL && list->tail == NULL);
		struct dlist_node *node = dlist_node_create(data);
		if (node == NULL) {
			return -1;
		}
		list->head = list->tail = node;
		list->size += 1;

		return list->size;
	}

	return dlist_ins_prev(list, list->head, data);
}

int dlist_append(struct dlist *list, void *data)
{
	if (list == NULL) {
		return -1;
	}
	if (list->tail == NULL) {
		assert(list->head == NULL && list->tail == NULL);
		struct dlist_node *node = dlist_node_create(data);
		if (node == NULL) {
			return -1;
		}
		list->head = list->tail = node;
		list->size += 1;

		return list->size;
	}

	return dlist_ins_next(list, list->tail, data);
}

int dlist_remove(struct dlist *list, struct dlist_node *elem,
	void **data, int need_free)
{
	if (list == NULL || elem == NULL) {
		return -1;
	}

	struct dlist_node *prev = elem->prev;
	struct dlist_node *next = elem->next;
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}

	if (elem == list->head) {
		list->head = next;
	}
	if (elem == list->tail) {
		list->tail = prev;	
	}

	if (data) {
		*data = elem->data;
	}
	if (need_free) {
		mp_free(g_mp, elem);
	} else {
		elem->prev = elem->next = NULL;
	}

	list->size -= 1;

	return list->size;
}

int dlist_pop_front(struct dlist *list, void **data)
{
	if (list == NULL) {
		return -1;
	}
		
	if (list->head == NULL) {
		assert(list->head == NULL && list->tail == NULL);

		return list->size;
	}
	
	return dlist_remove(list, list->head, data, 1);
}

int dlist_pop_back(struct dlist *list, void **data)
{
	if (list == NULL) {
		return -1;
	}

	if (list->tail == NULL) {
		assert(list->head == NULL && list->tail == NULL);
	
		return list->size;
	}

	return dlist_remove(list, list->tail, data, 1);
}

int dlist_size(struct dlist *list)
{
	if (list == NULL) {
		return 0;
	}

	return list->size;
}

struct dlist_node *dlist_head(struct dlist *list)
{
	if (list == NULL) {
		return NULL;
	}
	
	return list->head;
}

struct dlist_node *dlist_tail(struct dlist *list)
{
	if (list == NULL) {
		return NULL;
	}

	return list->tail;
}

int dlist_is_head(struct dlist_node *elem)
{
	return elem && elem->prev == NULL;
}

int dlist_is_tail(struct dlist_node *elem)
{
	return elem && elem->next == NULL;
}

void *dlist_data(struct dlist_node *elem)
{
	return elem ? elem->data : NULL;
}

struct dlist_node *dlist_next(struct dlist_node *elem)
{
	return elem ? elem->next : NULL;
}

struct dlist_node *dlist_prev(struct dlist_node *elem)
{
	return elem ? elem->prev : NULL;
}

void dlist_set_show(struct dlist *list, void (*show) (void *data))
{
	if (list == NULL) {
		return ;
	}
	list->show = show;
}

void dlist_show(struct dlist *list)
{
	if (list == NULL) {
		return ;
	}		
	struct dlist_node *p = list->head;
	printf("list: ( ");
	while (p) {
		list->show(p->data);
		p = p->next;
	}
	printf(" )\n");
}

int dlist_empty(struct dlist *list)
{
	if (list == NULL) {
		return 1;
	}
	if (list->size == 0) {
		assert(list->head == NULL && list->tail == NULL);
		return 1;
	}

	assert(list->head != NULL && list->tail != NULL);

	return 0;	
}

void dlist_sort(struct dlist *list, int (*cmp)(void *a, void *b), int asc)
{
	if (list == NULL || cmp == NULL) {
		return ;
	}

	int size = dlist_size(list);
	struct dlist_node *new_head = NULL, *new_tail = NULL, *node = NULL;
	while (dlist_size(list) > 0) {
		if (asc) {
			node = dlist_get_min(list, cmp);
		} else {
			node = dlist_get_max(list, cmp);
		}
		dlist_remove(list, node, NULL, 0);
		if (new_head == NULL) {
			new_head = new_tail = node;
			continue;
		}
		new_tail->next = node;
		node->prev = new_tail;
		new_tail = node;
	}
	assert(list->head == NULL && list->tail == NULL);
	list->head = new_head;
	list->tail = new_tail;
	list->size = size;
}

struct dlist_node *dlist_get_max(struct dlist *list, int (*cmp)(void *a, void *b))
{
	if (list == NULL || cmp == NULL) {
		return NULL;
	}
	struct dlist_node *node = NULL, *p = NULL;
	dlist_for_each(list, p) {
		if (node == NULL) {
			node = p;
			continue;
		}
		if (cmp(node->data, p->data) < 0) {
			node = p;
		}
	}

	return node;
}

struct dlist_node *dlist_get_min(struct dlist *list, int (*cmp)(void *a, void *b))
{
	if (list == NULL || cmp == NULL) {
		return NULL;
	}
	struct dlist_node *node = NULL, *p = NULL;
	dlist_for_each(list, p) {
		if (node == NULL) {
			node = p;
			continue;
		}
		if (cmp(node->data, p->data) > 0) {
			node = p;
		}
	}

	return node;
}

struct dlist *dlist_merge(struct dlist *list_a, struct dlist *list_b)
{
	if (list_a == NULL && list_b == NULL)
		return NULL;

	if (list_a == NULL || dlist_empty(list_a))
		return list_b;

	if (list_b == NULL || dlist_empty(list_b))
		return list_a;

	list_a->tail->next = list_b->head;
	list_b->head->prev = list_a->tail;

	list_a->tail = list_b->tail;

	list_a->size += list_b->size;

	return list_a;
}
