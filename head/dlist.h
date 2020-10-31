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
	dlist_node *head;
	dlist_node *tail;
} dlist;

void dlist_init(dlist *list, void (*destroy) (void *data));

void dlist_destroy(dlist *list);

int dlist_ins_next(dlist *list, dlist_node *elem, const void *data);

int dlist_ins_prev(dlist *list, dlist_node *elem, const void *data);

int dlist_remove(dlist *list, dlist_node *elem, void **data);
 


#endif
