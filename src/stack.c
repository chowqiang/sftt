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
#include <assert.h>
#include "debug.h"
#include "dlist.h"
#include "mem_pool.h"
#include "show.h"
#include "stack.h"

extern struct mem_pool *g_mp;

struct stack *stack_create(enum free_mode mode)
{
	DBUG_ENTER(__func__);

	struct stack *s = (struct stack *)mp_malloc(g_mp,
			__func__, sizeof(struct stack));
	if (s == NULL) {
		DBUG_RETURN(NULL);
	}

	s->list = dlist_create(mode);
	if (s->list == NULL) {
		mp_free(g_mp, s);
		DBUG_RETURN(NULL);
	}

	DBUG_RETURN(s);
}

void stack_init(struct stack *s, enum free_mode mode)
{
	DBUG_ENTER(__func__);

	assert(s != NULL);
	s->list = dlist_create(mode);

	DBUG_VOID_RETURN;
}

void stack_destroy(struct stack *s)
{
	DBUG_ENTER(__func__);

	assert(s != NULL);
	dlist_destroy(s->list);
	s->list = NULL;
	mp_free(g_mp, s);

	DBUG_VOID_RETURN;
}

int stack_push(struct stack *s, void *data)
{
	DBUG_ENTER(__func__);

	assert(s != NULL);

	DBUG_RETURN(dlist_prepend(s->list, data));
}

int stack_pop(struct stack *s, void **data)
{
	DBUG_ENTER(__func__);

	assert(s != NULL);

	DBUG_RETURN(dlist_pop_front(s->list, data));
}

void *stack_peek(const struct stack *s)
{
	DBUG_ENTER(__func__);

	assert(s != NULL);
	struct dlist_node *node = dlist_head(s->list);

	DBUG_RETURN(node == NULL ? NULL : dlist_data(node));
}

int stack_size(const struct stack *s)
{
	DBUG_ENTER(__func__);

	assert(s != NULL);

	DBUG_RETURN(dlist_size(s->list));
}

void stack_show(const struct stack *s, void (*show)(void *data))
{
	DBUG_ENTER(__func__);

	if (s == NULL || show == NULL) {
		DBUG_VOID_RETURN;
	}
	dlist_set_show(s->list, show);
	dlist_show(s->list);

	DBUG_VOID_RETURN;
}

int stack_is_empty(const struct stack *s)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(s == NULL || s->list == NULL ? 1 : dlist_size(s->list) == 0);
}

void stack_peek_all(const struct stack *s, void **array)
{
	DBUG_ENTER(__func__);

	int i = 0;
	struct dlist_node *node = NULL;
	dlist_for_each(s->list, node) {
		array[i] = node->data;
		++i;
	}

	DBUG_VOID_RETURN;
}

int stack_test(void)
{
	DBUG_ENTER(__func__);

	struct stack *s = stack_create(FREE_MODE_NOTHING);
	stack_push(s, (void *)1);
	stack_push(s, (void *)2);
	stack_push(s, (void *)3);
	stack_show(s, show_int);

	void *data = NULL;
	stack_pop(s, &data);
	printf("%ld\n", (unsigned long)data);
	if (s == NULL) {
		printf("unbelievable!\n");
		DBUG_RETURN(0);
	}
	stack_show(s, show_int);

	stack_pop(s, &data);
	printf("%ld\n", (unsigned long)data);
	stack_pop(s, &data);
	printf("%ld\n", (unsigned long)data);

	DBUG_RETURN(0);
}
