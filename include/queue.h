#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "dlist.h"

struct queue {
	struct dlist *list;
};

struct queue *queue_create(void (*destroy)(void *data));
void queue_init(struct queue *q, void (*destroy)(void *data));
void queue_destroy(struct queue *q);
int queue_enqueue(struct queue *q, void *data);
int queue_dequeue(struct queue *q, void **data);
void *queue_peek(struct queue *q);
int queue_size(struct queue *q);
int queue_is_empty(struct queue *q);

#endif
