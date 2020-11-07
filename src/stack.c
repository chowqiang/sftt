#include <stdlib.h>
#include <assert.h>
#include "stack.h"

void stack_init(stack *s, void (*destroy) (void *data)) {
	assert(s != NULL);
	s->list = dlist_create(destroy); 
}

void stack_destroy(stack *s) {
	assert(s != NULL);
	dlist_destroy(s->list);
	s->list = NULL;
}

int stack_push(stack *s, void *data) {
	assert(s != NULL);

	return dlist_prepend(s->list, data);
}

int stack_pop(stack *s, void **data) {
	assert(s != NULL);

	return dlist_pop_front(s->list, data);
}

void *stack_peek(const stack *s) {
	assert(s != NULL);
	dlist_node *node = dlist_head(s->list);

	return node == NULL ? NULL : dlist_data(node); 
}

int stack_size(const stack *s) {
	assert(s != NULL);

	return dlist_size(s->list);
}
