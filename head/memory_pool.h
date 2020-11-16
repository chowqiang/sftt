#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <stddef.h>
#include "dlist.h"

typedef struct {
	size_t size;
	int is_using;
	void *address;
	unsigned long long used_cnt;
} memory_node;

typedef struct {
	dlist *list;
} memory_pool;

memory_pool *mp_create();
memory_pool *get_singleton_mp();
int mp_init(memory_pool *mp);
void *mp_malloc(memory_pool *mp, size_t n);
void mp_free(memory_pool *mp, void *p);
void mp_destroy(memory_pool *mp);

#endif 
