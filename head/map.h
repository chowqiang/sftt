#ifndef _MAP_H_
#define _MAP_H_

#include "dlist.h"

typedef struct {
	dlist *list;
} map;

typedef struct {
	void *key;
	void *value;
} kv_node;

map *map_create();
int map_add(map *m, void *key, void *value);
int map_find(map *m, void *key, void **value);
int map_remove(map *m, void *key, void **value);
void *map_destroy(map *m);

#endif
