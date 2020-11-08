#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "dlist.h"
#include "destroy.h"
#include "show.h"

dlist_node *dlist_node_create(void *data) {
	dlist_node *node = (dlist_node *)malloc(sizeof(dlist_node));
	if (node == NULL) {
		return NULL;
	}

	node->data = data;
	node->prev = node->next = NULL;
	
	return node;
}

dlist *dlist_create(void (*destroy)(void *data)) {
	dlist *list = (dlist *)malloc(sizeof(dlist));
	dlist_init(list, destroy);

	return list;
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
		if (list->destroy) {
			list->destroy(p->data);	
		}
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

int dlist_remove(dlist *list, dlist_node *elem, void **data, int need_free) {
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

	if (elem == list->head) {
		list->head = next;
	}
	if (elem == list->tail) {
		list->tail = prev;	
	}

	if (data) {
		*data = elem->data;
	}
	if (need_free) {
		free(elem);
	} else {
		elem->prev = elem->next = NULL;
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
	
	return dlist_remove(list, list->head, data, 1);
}

int dlist_pop_back(dlist *list, void **data) {
	if (list == NULL) {
		return -1;
	}

	if (list->tail == NULL) {
		assert(list->head == NULL && list->tail == NULL);
	
		return list->size;
	}

	return dlist_remove(list, list->tail, data, 1);
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

int dlist_is_empty(dlist *list) {
	if (list == NULL) {
		return 1;
	}
	if (list->size == 0) {
		assert(list->head == NULL && list->tail == NULL);
		return 1;
	}

	assert(list->head != NULL && list->tail != NULL);

	return 0;	
}

void dlist_sort(dlist *list, int (*cmp)(void *a, void *b), int asc) {
	if (list == NULL || cmp == NULL) {
		return ;
	}

	int size = dlist_size(list);
	dlist_node *new_head = NULL, *new_tail = NULL, *node = NULL;
	while (dlist_size(list) > 0) {
		if (asc) {
			node = dlist_get_min(list, cmp);
		} else {
			node = dlist_get_max(list, cmp);
		}
		dlist_remove(list, node, NULL, 0);
		if (new_head == NULL) {
			new_head = new_tail = node;
			continue;
		}
		new_tail->next = node;
		node->prev = new_tail;
		new_tail = node;
	}
	assert(list->head == NULL && list->tail == NULL);
	list->head = new_head;
	list->tail = new_tail;
	list->size = size;
} 

dlist_node *dlist_get_max(dlist *list, int (*cmp)(void *a, void *b)) {
	if (list == NULL || cmp == NULL) {
		return ;
	}
	dlist_node *node = NULL, *p = NULL;
	dlist_for_each(list, p) {
		if (node == NULL) {
			node = p;
			continue;
		}
		if (cmp(node->data, p->data) < 0) {
			node = p;
		}
	}

	return node;
}

dlist_node *dlist_get_min(dlist *list, int (*cmp)(void *a, void *b)) {
	if (list == NULL || cmp == NULL) {
		return ;
	}
	dlist_node *node = NULL, *p = NULL;
	dlist_for_each(list, p) {
		if (node == NULL) {
			node = p;
			continue;
		}
		if (cmp(node->data, p->data) > 0) {
			node = p;
		}
	}

	return node;
}

int cmp_int(void *a, void *b) {
	return (int)a - (int)b;
}
#if 0
int main(void) {
	dlist list;
	dlist_init(&list, destroy_int);
	dlist_set_show(&list, show_int);
	dlist_show(&list);

	int size = dlist_prepend(&list, (void *)9);
	size = dlist_append(&list, (void *)5);
	size = dlist_ins_next(&list, dlist_tail(&list), (void *)2);
	size = dlist_ins_prev(&list, dlist_tail(&list), (void *)7);
	printf("%d, %d\n", size, dlist_size(&list));
	dlist_show(&list);

	dlist_sort(&list, cmp_int, 1);
	dlist_show(&list);
	printf("%d, %d\n", size, dlist_size(&list));

	size = dlist_pop_front(&list, NULL);
	size = dlist_remove(&list, dlist_head(&list), NULL, 1);
	size = dlist_remove(&list, dlist_head(&list), NULL, 1);
	size = dlist_pop_back(&list, NULL);
	printf("%d, %d\n", size, dlist_size(&list));
	dlist_show(&list);

	dlist_destroy(&list);

	return 0;
}
#endif
