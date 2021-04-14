#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <stddef.h>
#include "dlist.h"

struct mem_node {
	size_t size;
	int is_using;
	void *address;
	unsigned long long used_cnt;
};

struct mem_pool {
	struct dlist *list;
};

struct mem_pool *mem_pool_construct(void);
void mem_pool_deconstruct(struct mem_pool *ptr);
struct mem_pool *get_singleton_mp(void);
struct mem_pool *mp_create(void);
int mp_init(struct mem_pool *mp);
void *mp_malloc(struct mem_pool *mp, size_t n);
void mp_free(struct mem_pool *mp, void *p);
void mp_destroy(struct mem_pool *mp);

#endif 
