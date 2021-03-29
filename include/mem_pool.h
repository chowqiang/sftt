#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <stddef.h>
#include "dlist.h"

typedef struct {
	size_t size;
	int is_using;
	void *address;
	unsigned long long used_cnt;
} mem_node_t;

typedef struct {
	dlist *list;
} mem_pool_t;

mem_pool_t *mem_pool_t_construct(void);
void mem_pool_t_deconstruct(mem_pool_t *ptr);
mem_pool_t *get_singleton_mp(void);
mem_pool_t *mp_create(void);
int mp_init(mem_pool_t *mp);
void *mp_malloc(mem_pool_t *mp, size_t n);
void mp_free(mem_pool_t *mp, void *p);
void mp_destroy(mem_pool_t *mp);

#endif 
