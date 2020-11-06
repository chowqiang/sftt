#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

typedef struct {
	
} memory_pool;

int mp_init(memory_pool *mp);
void *mp_malloc(memory_pool *mp, size_t n);
void mp_free(memory_pool *mp, void *p);
void mp_destroy(memory_pool *mp)

#endif 
