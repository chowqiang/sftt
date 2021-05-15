#include <stdio.h>
#include <stdlib.h>
#include "map.h"

int map_init(struct map *m)
{
	if (m == NULL) {
		return -1;
	}

	m->list = dlist_create(free);
	if (m->list == NULL) {
		free(m);
		return -1;
	}

	return 0;
}

struct map *map_create(void){
	struct map *m = (struct map *)malloc(sizeof(struct map));
	if (m == NULL) {
		return NULL;
	}
	m->list = dlist_create(free);
	if (m->list == NULL) {
		free(m);
		return NULL;
	}

	return m;
}
int map_add(struct map *m, void *key, void *value){
	if (m == NULL) {
		printf("map object cannot be NULL\n");
		return -1;
	}

	struct kv_node *data = (struct kv_node *)malloc(sizeof(struct kv_node));
	if (data == NULL) {
		printf("cannot alloc kv_node object\n");
		return -1;
	} 

	data->key = key;
	data->value = value;

	struct kv_node *kvn = NULL;
	struct dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (struct kv_node *)ln->data;
		if (kvn->key == data->key) {
			kvn->value = data->value;
			free(data);
			return 0;
		}
	} 

	dlist_append(m->list, (void *)data);
	return 0;
}

int map_find(struct map *m, key_equal_t is_equal, void *key, void **value){
	if (m == NULL) {
		printf("map object cannot be NULL\n");
		return -1; 
	}
	struct kv_node *kvn = NULL;
	struct dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (struct kv_node *)ln->data;
		if (is_equal(kvn->key, key)) {
			if (value) {
				*value = kvn->value;
			}
			return 0;
		}
	}

	printf("cannot find elem!\n");
	return -1;
}

int map_remove(struct map *m, void *key, void **value){
	if (m == NULL) {
		return -1;
	}
	struct kv_node *kvn = NULL;
	struct dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (struct kv_node *)ln->data;
		if (kvn->key == key) {
			dlist_remove(m->list, ln, value, 1); 			
			return 0;
		}
	}
	return -1;
}

void map_destroy(struct map *m){
	if (m == NULL || m->list == NULL) {
		return ;
	} 	

	dlist_destroy(m->list);
}
