#include <assert.h> 
#include <stdlib.h>
#include "queue.h"
#include "show.h"

queue *queue_create(void (*destroy)(void *data)) {
	queue *q = (queue *)malloc(sizeof(queue));
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

void queue_show(queue *q, void (*show)(void *data)) {
	assert(q != NULL);
	
	dlist_set_show(q->list, show);
	dlist_show(q->list); 
}

int queue_is_empty(queue *q) {
	return q == NULL || q->list == NULL ? 0 : dlist_is_empty(q->list);
}

#if 0
int main(void) {
	queue *q = queue_create(NULL);

	queue_enqueue(q, (void *)1);
	queue_enqueue(q, (void *)2);
	queue_enqueue(q, (void *)3);

	queue_show(q, show_int);

	void *data = NULL;
	queue_dequeue(q, &data);
	printf("%d\n", (int)data);
	queue_show(q, show_int);

	queue_dequeue(q, &data);
	printf("%d\n", (int)data);
	queue_show(q, show_int);
	
	queue_dequeue(q, &data);
	printf("%d\n", (int)data);
	queue_show(q, show_int);
		
	return 0;
}
#endif
