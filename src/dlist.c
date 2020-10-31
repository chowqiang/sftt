#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "dlist.h"

dlist_node *dlist_node_create(void *data) {
	dlist_node *node = (dlist_node *)malloc(sizeof(dlist_node));
	if (node == NULL) {
		return NULL;
	}

	node->data = data;
	node->prev = node->next = NULL;
	
	return node;
}

void dlist_init(dlist *list, void (*destroy) (void *data)) {	
	if (list == NULL) {
		return ;
	}

	list->size = 0;
	list->destroy = destroy;
	list->show = NULL;
	list->head = NULL;
	list->tail = NULL;
}

void dlist_destroy(dlist *list) {
	if (list == NULL || list->destroy == NULL) {
		return ;
	}

	dlist_node *p = list->head;
	dlist_node *q = NULL;
	while (p) {
		list->destroy(p->data);	
		q = p->next;
		free(p);
		p = q;
	}
}

int dlist_ins_next(dlist *list, dlist_node *elem, void *data) {
	if (list == NULL || elem == NULL) {
		return -1;
	}

	dlist_node *node = dlist_node_create(data);
	if (node == NULL) {
		return -1;
	} 

	dlist_node *next = elem->next;
	node->next = next;
	if (next) {
		next->prev = node;
	}
	node->prev = elem;
	elem->next = node;
	
	list->size += 1;

	if (list->tail == elem) {
		list->tail = node;
	}

	return list->size;
}

int dlist_ins_prev(dlist *list, dlist_node *elem, void *data) {
	if (list == NULL || elem == NULL) {
		return -1;
	}
	
	dlist_node *node = dlist_node_create(data);
	if (node == NULL) {
		return -1;
	}

	dlist_node *prev = elem->prev;
	node->prev = prev;
	if (prev) {
		prev->next = node;
	}
	node->next = elem;
	elem->prev = node;

	list->size += 1;

	if (list->head == elem) {
		list->head = node;
	}
	
	return list->size;
}

int dlist_prepend(dlist *list, void *data) {
	if (list == NULL) {
		return -1;
	}
	if (list->head == NULL) {
		assert(list->head == NULL && list->tail == NULL);
		dlist_node *node = dlist_node_create(data);
		if (node == NULL) {
			return -1;
		}
		list->head = list->tail = node;
		list->size += 1;

		return list->size;
	}

	return dlist_ins_prev(list, list->head, data); 
}

int dlist_append(dlist *list, void *data) {
	if (list == NULL) {
		return -1;
	}
	if (list->tail == NULL) {
		assert(list->head == NULL && list->tail == NULL);
		dlist_node *node = dlist_node_create(data);
		if (node == NULL) {
			return -1;
		}
		list->head = list->tail = node;
		list->size += 1;

		return list->size;
	}

	return dlist_ins_next(list, list->tail, data); 
}

int dlist_remove(dlist *list, dlist_node *elem, void **data) {
	if (list == NULL || elem == NULL) {
		return -1;
	} 

	dlist_node *prev = elem->prev;
	dlist_node *next = elem->next;
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}

	if (data && *data) {
		*data = elem->data;
	}
	free(elem);

	if (elem == list->head) {
		list->head = next;
	}
	if (elem == list->tail) {
		list->tail = prev;	
	}
	list->size -= 1;

	return list->size;
}

int dlist_pop_front(dlist *list, void **data) {
	if (list == NULL) {
		return -1;
	}
		
	if (list->head == NULL) {
		assert(list->head == NULL && list->tail == NULL);

		return list->size;
	}
	
	return dlist_remove(list, list->head, data);	
}

int dlist_pop_back(dlist *list, void **data) {
	if (list == NULL) {
		return -1;
	}

	if (list->tail == NULL) {
		assert(list->head == NULL && list->tail == NULL);
	
		return list->size;
	}

	return dlist_remove(list, list->tail, data);
}

int dlist_size(dlist *list) {
	if (list == NULL) {
		return 0;
	}

	return list->size;
}

dlist_node *dlist_head(dlist *list) {
	if (list == NULL) {
		return NULL;
	}
	
	return list->head;
}

dlist_node *dlist_tail(dlist *list) {
	if (list == NULL) {
		return NULL;
	}

	return list->tail;
}

int dlist_is_head(dlist_node *elem) {
	return elem && elem->prev == NULL;
}

int dlist_is_tail(dlist_node *elem) {
	return elem && elem->next == NULL;
}

void *dlist_data(dlist_node *elem) {
	return elem ? elem->data : NULL;
}

dlist_node *dlist_next(dlist_node *elem) {
	return elem ? elem->next : NULL;
}

dlist_node *dlist_prev(dlist_node *elem) {
	return elem ? elem->prev : NULL;
}

void dlist_set_show(dlist *list, void (*show) (void *data)) {
	if (list == NULL) {
		return ;
	} 
	list->show = show;
}

void dlist_show(dlist *list) {
	if (list == NULL) {
		return ;
	}		
	dlist_node *p = list->head;
	printf("list: ( ");
	while (p) {
		list->show(p->data);
		p = p->next;
	}
	printf(" )\n");
}

void destroy_int(void *data) {
	return ;
}

void show_int(void *data) {
	if (data == NULL) {
		return ;
	}
	printf("%d ", (int)(data));
}
#if 1

int main(void) {
	dlist list;
	dlist_init(&list, destroy_int);
	dlist_set_show(&list, show_int);
	dlist_show(&list);

	int size = dlist_prepend(&list, (void *)1);
	size = dlist_append(&list, (void *)2);
	size = dlist_ins_next(&list, dlist_tail(&list), (void *)4);
	size = dlist_ins_prev(&list, dlist_tail(&list), (void *)3);
	printf("%d, %d\n", size, dlist_size(&list));
	dlist_show(&list);

	size = dlist_pop_front(&list, NULL);	
	size = dlist_remove(&list, dlist_head(&list), NULL);	
	size = dlist_remove(&list, dlist_head(&list), NULL);	
	size = dlist_pop_back(&list, NULL);	
	printf("%d, %d\n", size, dlist_size(&list));
	dlist_show(&list);

	dlist_destroy(&list);	

	return 0;
}
#endif
