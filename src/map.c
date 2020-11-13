#include <stdlib.h>
#include "map.h"

map *map_create(){
	map *m = (map *)malloc(sizeof(map));
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
int map_add(map *m, void *key, void *value){
	if (m == NULL) {
		return -1;
	}
	kv_node *data = (kv_node *)malloc(sizeof(kv_node));
	if (data == NULL) {
		return -1;
	} 
	data->key = key;
	data->value = value;

	kv_node *kvn = NULL;
	dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (kv_node *)ln->data;
		if (kvn->key == data->key) {
			kvn->value = data->value;
			free(data);
			return 0;
		}
	} 

	dlist_append(m->list, (void *)data);
	return 0;
}

int map_find(map *m, void *key, void **value){
	if (m == NULL) {
		return -1; 
	}
	kv_node *kvn = NULL;
	dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (kv_node *)ln->data;
		if (kvn->key == key) {
			if (value) {
				*value = kvn->value;
			}
			return 0;
		}
	}

	return -1;
}

int map_remove(map *m, void *key, void **value){
	if (m == NULL) {
		return -1;
	}
	kv_node *kvn = NULL;
	dlist_node *ln = NULL;
	dlist_for_each(m->list, ln) {
		kvn = (kv_node *)ln->data;
		if (kvn->key == key) {
			dlist_remove(m->list, ln, value, 1); 			
			return 0;
		}
	}
	return -1;
}

void map_destroy(map *m){
	if (m == NULL || m->list == NULL) {
		return ;
	} 	

	dlist_destroy(m->list);
}
