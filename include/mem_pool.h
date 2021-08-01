/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <pthread.h>
#include <stddef.h>
#include "list.h"
#include "lock.h"
#include "shm_space.h"

#define MEM_POOL_STAT "mem_pool_stat"

#define MEM_POOL_STAT_MSGKEY "/var/lib/sftt/mps_msgkey"

struct mem_node {
	/* Memory size of this node */
	size_t size;
	int is_using;
	void *address;
	unsigned long long used_cnt;
	struct list_head list;
};

struct mem_pool_stat {
	int total_size;
	int total_nodes;
	int using_nodes;
	int free_nodes;
};

struct mem_pool {
	/*
	 * Memory pool contains a list of mem_node
	 * The mutex prevent race.
	 */
	struct mem_node *nodes;
	struct pthread_mutex *mutex;
	struct mem_pool_stat stat;
	struct msg_queue *msg_queue;
	pthread_t thread_mps;
};

struct mem_pool *mem_pool_construct(void);
void mem_pool_destruct(struct mem_pool *ptr);
struct mem_pool *get_singleton_mp(void);
struct mem_pool *mp_create(void);
void *mp_malloc(struct mem_pool *mp, size_t n);
void *mp_realloc(struct mem_pool *mp, void *addr, size_t n);
void mp_stat(struct mem_pool *mp);
void mp_free(struct mem_pool *mp, void *p);
void mp_destroy(struct mem_pool *mp);

#endif 
