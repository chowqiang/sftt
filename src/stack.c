#include <stdlib.h>
#include <assert.h>
#include "stack.h"
#include "dlist.h"

stack *stack_create(void (*destroy)(void *data)) {
	stack *s = (stack *)malloc(sizeof(stack));
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

#if 1
int main(void) {
	stack *s = stack_create(NULL);
	stack_push(s, (void *)1);
	stack_push(s, (void *)2);
	stack_push(s, (void *)3);

	int data = 0;
	stack_pop(s, (void **)&data);
	printf("%d\n", data);
	stack_pop(s, (void **)&data);
	printf("%d\n", data);
	stack_pop(s, (void **)&data);
	printf("%d\n", data);

	return 0;
}
#endif
