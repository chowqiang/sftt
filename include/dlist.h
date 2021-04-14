#ifndef _DLIST_H_
#define _DLIST_H_

struct dlist_node {
	void *data;
	struct dlist_node *prev;
	struct dlist_node *next;
};

struct dlist {
	int size;
	void (*destroy) (void *data);
	void (*show) (void *data);
	struct dlist_node *head;
	struct dlist_node *tail;
};

struct dlist_node *dlist_node_create(void *data); 

struct dlist *dlist_create(void (*destroy)(void *data)); 

void dlist_init(struct dlist *list, void (*destroy) (void *data));

void dlist_set_destroy(struct dlist *list, void (*destroy)(void *data));

void dlist_destroy(struct dlist *list);

int dlist_ins_next(struct dlist *list, struct dlist_node *elem, void *data);

int dlist_ins_prev(struct dlist *list, struct dlist_node *elem, void *data);

int dlist_append(struct dlist *list, void *data);

int dlist_prepend(struct dlist *list, void *data);

int dlist_remove(struct dlist *list, struct dlist_node *elem, void **data, int need_free);

int dlist_size(struct dlist *list);

struct dlist_node *dlist_head(struct dlist *list);

struct dlist_node *dlist_tail(struct dlist *list);

int dlist_is_head(struct dlist_node *elem);

int dlist_is_tail(struct dlist_node *elem);

void *dlist_data(struct dlist_node *elem);

struct dlist_node *dlist_next(struct dlist_node *elem);

struct dlist_node *dlist_prev(struct dlist_node *elem);

int dlist_pop_front(struct dlist *list, void **data); 

int dlist_pop_back(struct dlist *list, void **data); 

void dlist_set_show(struct dlist *list, void (*show) (void *data));

void dlist_show(struct dlist *list);
 
int dlist_is_empty(struct dlist *list);

void dlist_sort(struct dlist *list, int (*cmp)(void *a, void *b), int asc);

struct dlist_node *dlist_get_max(struct dlist *list, int (*cmp)(void *a, void *b));

struct dlist_node *dlist_get_min(struct dlist *list, int (*cmp)(void *a, void *b));

#define dlist_for_each(list, node) \
	for (node = list->head; node != NULL; node = node->next)

#define dlist_for_each_pos(node) \
	for (; node != NULL; node = node->next)

#endif
