#include <assert.h> 
#include <stdlib.h>
#include "queue.h"

void queue_init(queue *q, void (*destroy)(void *data)) {
	assert(q != NULL);
	q->list = dlist_create(destroy);
}

void queue_destroy(queue *q) {
	assert(q != NULL);
	dlist_destroy(q->list);	
	q->list = NULL;
}

int queue_enqueue(queue *q, void *data) {
	assert(q != NULL);

	return dlist_append(q->list, data);
}

int queue_dequeue(queue *q, void **data) {
	assert(q != NULL);

	return dlist_pop_front(q->list, data);
}

void *queue_peek(queue *q) {
	assert(q != NULL);
	dlist_node *head = dlist_head(q->list);

	return head == NULL ? NULL : dlist_data(head);
}

int queue_size(queue *q) {
	assert(q != NULL);

	return dlist_size(q->list);
}
