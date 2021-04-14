#ifndef _MAP_H_
#define _MAP_H_

#include "dlist.h"

struct map {
	struct dlist *list;
};

struct kv_node {
	void *key;
	void *value;
};

struct map *map_create();
int map_add(struct map *m, void *key, void *value);
int map_find(struct map *m, void *key, void **value);
int map_remove(struct map *m, void *key, void **value);
void map_destroy(struct map *m);

#endif
