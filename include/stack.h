#ifndef _STACK_H_
#define _STACK_H_

#include "dlist.h"

struct stack {
	struct dlist *list;
};

struct stack *stack_create(void (*destroy)(void *data));
void stack_init(struct stack *s, void (*destroy) (void *data));
void stack_destroy(struct stack *s);
int stack_push(struct stack *s, void *data);
int stack_pop(struct stack *s, void **data);
void *stack_peek(const struct stack *s);
int stack_size(const struct stack *s);
int stack_is_empty(const struct stack *s);
void stack_peek_all(const struct stack *s, void **array);

#endif
