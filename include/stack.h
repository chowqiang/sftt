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

#ifndef _STACK_H_
#define _STACK_H_

#include "dlist.h"

struct stack {
	struct dlist *list;
};

struct stack *stack_create(enum free_mode mode);
void stack_init(struct stack *s, enum free_mode mode);
void stack_destroy(struct stack *s);
int stack_push(struct stack *s, void *data);
int stack_pop(struct stack *s, void **data);
void *stack_peek(const struct stack *s);
int stack_size(const struct stack *s);
int stack_is_empty(const struct stack *s);
void stack_peek_all(const struct stack *s, void **array);

#endif
