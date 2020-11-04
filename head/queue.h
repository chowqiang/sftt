#ifndef _QUEUE_H_
#define _QUEUE_H_
typedef struct {
	void *buffer;
	int head;
	int tail;
} queue;
void queue_init(queue *q, void (*destroy)(void *data));
void queue_destroy(queue *q);
int queue_enqueue(queue *q, void *data);
int queue_dequeue(queue *q, void **data);
void *queue_peek(queue *q);
int queue_size(queue *q);

#endif
