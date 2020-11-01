#ifndef _DLIST_H_
#define _DLIST_H_

typedef struct _dlist_node {
	void *data;
	struct _dlist_node *prev;
	struct _dlist_node *next;
} dlist_node;

typedef struct _dlist {
	int size;
	void (*destroy) (void *data);
	void (*show) (void *data);
	dlist_node *head;
	dlist_node *tail;
} dlist;

dlist_node *dlist_node_create(void *data); 

void dlist_init(dlist *list, void (*destroy) (void *data));

void dlist_destroy(dlist *list);

int dlist_ins_next(dlist *list, dlist_node *elem, void *data);

int dlist_ins_prev(dlist *list, dlist_node *elem, void *data);

int dlist_append(dlist *list, void *data);

int dlist_prepend(dlist *list, void *data);

int dlist_remove(dlist *list, dlist_node *elem, void **data);

int dlist_size(dlist *list);

dlist_node *dlist_head(dlist *list);

dlist_node *dlist_tail(dlist *list);

int dlist_is_head(dlist_node *elem);

int dlist_is_tail(dlist_node *elem);

void *dlist_data(dlist_node *elem);

dlist_node *dlist_next(dlist_node *elem);

dlist_node *dlist_prev(dlist_node *elem);

int dlist_pop_front(dlist *list, void **data); 

int dlist_pop_back(dlist *list, void **data); 

void dlist_set_show(dlist *list, void (*show) (void *data));

void dlist_show(dlist *list);
 
int dlist_is_empty(dlist *list);
#endif
