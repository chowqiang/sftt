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

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "dlist.h"

struct queue {
	struct dlist *list;
};

struct queue *queue_create(enum free_mode mode);
void queue_init(struct queue *q, enum free_mode mode);
void queue_destroy(struct queue *q);
int queue_enqueue(struct queue *q, void *data);
int queue_dequeue(struct queue *q, void **data);
void *queue_peek(struct queue *q);
int queue_size(struct queue *q);
int queue_is_empty(struct queue *q);

#endif
