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

#ifndef _DLIST_H_
#define _DLIST_H_

enum free_mode {
	FREE_MODE_NOTHING,
	FREE_MODE_MP_FREE,
	FREE_MODE_FREE
};

/*
 * The node of double list
 */
struct dlist_node {
	/* the data of this node which can be anything */
	void *data;
	struct dlist_node *prev;
	struct dlist_node *next;
};

/*
 * The double list
 */
struct dlist {
	int size;
	enum free_mode free_mode;

	/* print the data of node */
	void (*show) (void *data);

	struct dlist_node *head;
	struct dlist_node *tail;
};

struct dlist_node *dlist_node_create(void *data);
struct dlist *dlist_create(enum free_mode mode);
void dlist_init(struct dlist *list, enum free_mode mode);
void dlist_destroy(struct dlist *list);

int dlist_ins_next(struct dlist *list, struct dlist_node *elem, void *data);
int dlist_ins_prev(struct dlist *list, struct dlist_node *elem, void *data);
int dlist_append(struct dlist *list, void *data);
int dlist_prepend(struct dlist *list, void *data);
int dlist_remove(struct dlist *list, struct dlist_node *elem, void **data, int need_free);

struct dlist_node *dlist_head(struct dlist *list);
struct dlist_node *dlist_tail(struct dlist *list);

int dlist_is_head(struct dlist_node *elem);
int dlist_is_tail(struct dlist_node *elem);

void *dlist_data(struct dlist_node *elem);
struct dlist_node *dlist_next(struct dlist_node *elem);
struct dlist_node *dlist_prev(struct dlist_node *elem);

int dlist_pop_front(struct dlist *list, void **data);
int dlist_pop_back(struct dlist *list, void **data);

void dlist_set_show(struct dlist *list, void (*show) (void *data));
void dlist_show(struct dlist *list);

int dlist_empty(struct dlist *list);
int dlist_size(struct dlist *list);

struct dlist_node *dlist_get_max(struct dlist *list, int (*cmp)(void *a, void *b));
struct dlist_node *dlist_get_min(struct dlist *list, int (*cmp)(void *a, void *b));

struct dlist *dlist_merge(struct dlist *list_a, struct dlist *list_b);
void dlist_sort(struct dlist *list, int (*cmp)(void *a, void *b), int asc);

#define dlist_for_each(list, node) \
	for (node = list->head; node != NULL; node = node->next)

#define dlist_for_each_pos(node) \
	for (; node != NULL; node = node->next)

#endif
