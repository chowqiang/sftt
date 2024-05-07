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
#include "debug.h"
#include "mem_pool.h"
#include "queue.h"
#include "show.h"

struct queue *queue_create(enum free_mode mode)
{
	DBUG_ENTER(__func__);

	struct queue *q = (struct queue *)mp_malloc(g_mp, __func__,
			sizeof(struct queue));
	if (q == NULL) {
		DBUG_RETURN(NULL);
	}
	q->list = dlist_create(mode);
	if (q->list == NULL) {
		mp_free(g_mp, q);
		DBUG_RETURN(NULL);
	}

	DBUG_RETURN(q);
}

void queue_init(struct queue *q, enum free_mode mode)
{
	DBUG_ENTER(__func__);

	assert(q != NULL);
	q->list = dlist_create(mode);

	DBUG_VOID_RETURN;
}

void queue_destroy(struct queue *q)
{
	DBUG_ENTER(__func__);

	assert(q != NULL);
	dlist_destroy(q->list);
	q->list = NULL;
	mp_free(g_mp, q);

	DBUG_VOID_RETURN;
}

int queue_enqueue(struct queue *q, void *data)
{
	DBUG_ENTER(__func__);

	assert(q != NULL);

	DBUG_RETURN(dlist_append(q->list, data));
}

int queue_dequeue(struct queue *q, void **data)
{
	DBUG_ENTER(__func__);

	assert(q != NULL);

	DBUG_RETURN(dlist_pop_front(q->list, data));
}

void *queue_peek(struct queue *q)
{
	DBUG_ENTER(__func__);

	assert(q != NULL);
	struct dlist_node *head = dlist_head(q->list);

	DBUG_RETURN(head == NULL ? NULL : dlist_data(head));
}

int queue_size(struct queue *q)
{
	DBUG_ENTER(__func__);

	assert(q != NULL);

	DBUG_RETURN(dlist_size(q->list));
}

void queue_show(struct queue *q, void (*show)(void *data))
{
	DBUG_ENTER(__func__);

	assert(q != NULL);

	dlist_set_show(q->list, show);
	dlist_show(q->list);

	DBUG_VOID_RETURN;
}

int queue_is_empty(struct queue *q)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(q == NULL || q->list == NULL ? 0 : dlist_empty(q->list));
}

int queue_test(void)
{
	DBUG_ENTER(__func__);

	struct queue *q = queue_create(FREE_MODE_NOTHING);

	queue_enqueue(q, (void *)1);
	queue_enqueue(q, (void *)2);
	queue_enqueue(q, (void *)3);

	queue_show(q, show_int);

	void *data = NULL;
	queue_dequeue(q, &data);
	printf("%ld\n", (unsigned long)data);
	queue_show(q, show_int);

	queue_dequeue(q, &data);
	printf("%ld\n", (unsigned long)data);
	queue_show(q, show_int);

	queue_dequeue(q, &data);
	printf("%ld\n", (unsigned long)data);
	queue_show(q, show_int);

	DBUG_RETURN(0);
}
