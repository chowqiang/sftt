#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_
#include <stddef.h>

typedef struct {
	size_t size;
	int is_using;
	void *address;
	unsigned long long used_cnt;
} memory_node;

typedef struct {
	dlist *list;
	size_t node_cnt;	
} memory_pool;

int mp_init(memory_pool *mp);
void *mp_malloc(memory_pool *mp, size_t n);
void mp_free(memory_pool *mp, void *p);
void mp_destroy(memory_pool *mp);

#endif 
