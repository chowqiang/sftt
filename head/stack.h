#ifndef _STACK_H_
#define _STACK_H_
#include "dlist.h"
typedef struct {
	dlist *list;
} stack;

stack *stack_create(void (*destroy)(void *data));
void stack_init(stack *s, void (*destroy) (void *data));
void stack_destroy(stack *s);
int stack_push(stack *s, void *data);
int stack_pop(stack *s, void **data);
void *stack_peek(const stack *s);
int stack_size(const stack *s);
int stack_is_empty(const stack *s);
void stack_peek_all(const stack *s, void **array);

#endif
