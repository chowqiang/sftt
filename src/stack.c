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
#include "stack.h"
#include "dlist.h"
#include "show.h"

struct stack *stack_create(void (*destroy)(void *data)) {
	struct stack *s = (struct stack *)malloc(sizeof(struct stack));
	if (s == NULL) {
		return NULL;
	}

	s->list = dlist_create(destroy);
	if (s->list == NULL) {
		free(s);
		return NULL;
	}

	return s;
}

void stack_init(struct stack *s, void (*destroy) (void *data)) {
	assert(s != NULL);
	s->list = dlist_create(destroy); 
}

void stack_destroy(struct stack *s) {
	assert(s != NULL);
	dlist_destroy(s->list);
	s->list = NULL;
}

int stack_push(struct stack *s, void *data) {
	assert(s != NULL);

	return dlist_prepend(s->list, data);
}

int stack_pop(struct stack *s, void **data) {
	assert(s != NULL);

	return dlist_pop_front(s->list, data);
}

void *stack_peek(const struct stack *s) {
	assert(s != NULL);
	struct dlist_node *node = dlist_head(s->list);

	return node == NULL ? NULL : dlist_data(node); 
}

int stack_size(const struct stack *s) {
	assert(s != NULL);

	return dlist_size(s->list);
}

void stack_show(const struct stack *s, void (*show)(void *data)) {
	if (s == NULL || show == NULL) {
		return ;
	}	
	dlist_set_show(s->list, show);
	dlist_show(s->list);
}

int stack_is_empty(const struct stack *s) {
	return s == NULL || s->list == NULL ? 1 : dlist_size(s->list) == 0;
}

void stack_peek_all(const struct stack *s, void **array) {
	int i = 0;
	struct dlist_node *node = NULL;
	dlist_for_each(s->list, node) {
		array[i] = node->data;
		++i;
	} 
}

int stack_test(void) {
	struct stack *s = stack_create(NULL);
	stack_push(s, (void *)1);
	stack_push(s, (void *)2);
	stack_push(s, (void *)3);
	stack_show(s, show_int);

	void *data = NULL;
	stack_pop(s, &data);
	printf("%d\n", data);
	if (s == NULL) {
		printf("unbelievable!\n");
		return 0;
	}
	stack_show(s, show_int);

	stack_pop(s, &data);
	printf("%d\n", data);
	stack_pop(s, &data);
	printf("%d\n", data);

	return 0;
}
