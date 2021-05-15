#ifndef _MAP_H_
#define _MAP_H_

#include <stdbool.h>
#include <string.h>
#include "dlist.h"

struct map {
	struct dlist *list;
};

struct kv_node {
	void *key;
	void *value;
};

typedef bool (key_equal_t)(void *, void *);

static inline bool ptr_equal(void *a, void *b)
{
	return a == b;
}

static inline bool str_equal(void *a, void *b)
{
	if (a == NULL || b == NULL)
		return false;

	return strcmp((char *)a, (char *)b) == 0;
}

struct map *map_create();
int map_init(struct map *m);
int map_add(struct map *m, void *key, void *value);
int map_find(struct map *m, key_equal_t is_equal, void *key, void **value);
int map_remove(struct map *m, void *key, void **value);
void map_destroy(struct map *m);

#endif
