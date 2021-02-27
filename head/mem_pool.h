#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <stddef.h>
#include "dlist.h"

typedef struct {
	size_t size;
	int is_using;
	void *address;
	unsigned long long used_cnt;
} mem_node;

typedef struct {
	dlist *list;
} mem_pool;

mem_pool *mp_create(void);
mem_pool *get_singleton_mp(void);
int mp_init(mem_pool *mp);
void *mp_malloc(mem_pool *mp, size_t n);
void mp_free(mem_pool *mp, void *p);
void mp_destroy(mem_pool *mp);

#endif 
