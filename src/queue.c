#include <assert.h> 
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "show.h"

struct queue *queue_create(void (*destroy)(void *data)) {
	struct queue *q = (struct queue *)malloc(sizeof(struct queue));
	if (q == NULL) {
		return NULL;
	}
	q->list = dlist_create(destroy);
	if (q->list == NULL) {
		free(q);
		return NULL;
	}

	return q;
}

void queue_init(struct queue *q, void (*destroy)(void *data)) {
	assert(q != NULL);
	q->list = dlist_create(destroy);
}

void queue_destroy(struct queue *q) {
	assert(q != NULL);
	dlist_destroy(q->list);	
	q->list = NULL;
}

int queue_enqueue(struct queue *q, void *data) {
	assert(q != NULL);

	return dlist_append(q->list, data);
}

int queue_dequeue(struct queue *q, void **data) {
	assert(q != NULL);

	return dlist_pop_front(q->list, data);
}

void *queue_peek(struct queue *q) {
	assert(q != NULL);
	struct dlist_node *head = dlist_head(q->list);

	return head == NULL ? NULL : dlist_data(head);
}

int queue_size(struct queue *q) {
	assert(q != NULL);

	return dlist_size(q->list);
}

void queue_show(struct queue *q, void (*show)(void *data)) {
	assert(q != NULL);
	
	dlist_set_show(q->list, show);
	dlist_show(q->list); 
}

int queue_is_empty(struct queue *q) {
	return q == NULL || q->list == NULL ? 0 : dlist_is_empty(q->list);
}

int queue_test(void) {
	struct queue *q = queue_create(NULL);

	queue_enqueue(q, (void *)1);
	queue_enqueue(q, (void *)2);
	queue_enqueue(q, (void *)3);

	queue_show(q, show_int);

	void *data = NULL;
	queue_dequeue(q, &data);
	printf("%d\n", data);
	queue_show(q, show_int);

	queue_dequeue(q, &data);
	printf("%d\n", data);
	queue_show(q, show_int);
	
	queue_dequeue(q, &data);
	printf("%d\n", data);
	queue_show(q, show_int);
		
	return 0;
}
