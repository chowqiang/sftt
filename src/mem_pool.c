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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "base.h"
#include "context.h"
#include "debug.h"
#include "mem_pool.h"
#include "msg_queue.h"
#include "utils.h"

#ifdef CONFIG_MP_STAT_DEBUG
void *mp_update_stat_loop(void *arg);
#endif

/*
 * The global mem pool pointer
 */
struct mem_pool *g_mp = NULL;

static bool updated = false;

static LIST_HEAD(mem_nodes);

static LIST_HEAD(purposes);

int get_purpose_count(struct mem_pool *mp);
struct purpose_node *find_purpose(struct mem_pool *mp, const char *purpose);
void add_purpose(struct mem_pool *mp, const char *purpose);
void sub_purpose(struct mem_pool *mp, const char *purpose);

/*
 * Make the g_mp available
 */
static void __attribute__((constructor)) mem_pool_init(void)
{
	get_singleton_mp();	
}

/*
 */
static void __attribute__((destructor)) mem_pool_del(void)
{
	if (g_mp == NULL)
		return;
}

/*
 * Return the g_mp and initialise it if necessary
 */
struct mem_pool *get_singleton_mp()
{
	if (g_mp) {
		return g_mp;
	}

	g_mp = mp_create();

	return g_mp;
}

struct purpose_node *purpose_node_create(const char *purpose)
{
	struct purpose_node *p = (struct purpose_node *)malloc(
			sizeof(struct purpose_node));

	if (p == NULL)
		return NULL;

	p->purpose = purpose;
	p->count = 0;

	INIT_LIST_HEAD(&p->list);

	return p;
}

/*
 * Create a mem node
 * @size: The size of mem node
 *
 * Return: mem node pointer
 */
struct mem_node *mem_node_create(size_t size)
{
	struct mem_node *p = (struct mem_node *)malloc(sizeof(struct mem_node));

	if (p == NULL) {
		return NULL;
	}

	p->address = malloc(size);
	if (p->address == NULL) {
		free(p);
		return NULL;
	}

	p->size = size;
	p->is_using = 0;
	p->used_cnt = 0;

	INIT_LIST_HEAD(&p->list);

	return p;
}

/*
 * Free a mem node
 * @data: The mem node pointer to be freed
 */
void mem_node_free(void *data)
{
	struct mem_node *node = (struct mem_node *)data;

	if (data == NULL) {
		return ;
	}

	if (!node->is_using && node->address) {
		free(node->address);
	}
}

/*
 * Create a mem pool
 *
 * Return: The mem pool pointer created
 */
struct mem_pool *mp_create(void)
{
	int ret;
	struct mem_pool *mp = (struct mem_pool *)malloc(sizeof(struct mem_pool));

	if (mp == NULL) {
		return NULL;
	}
	mp->nodes = NULL;

	mp->mutex = new(pthread_mutex);
	assert(mp->mutex != NULL);

	mp->stat.total_size = 0;
	mp->stat.total_nodes = 0;
	mp->stat.using_nodes = 0;
	mp->stat.free_nodes = 0;

	mp->msg_queue = NULL;

#ifdef CONFIG_MP_STAT_DEBUG
	ret = pthread_create(&mp->thread_mps, NULL, mp_update_stat_loop, mp);
	if (ret) {
		printf("create thread for mem_pool failed\n");
	}
#endif

	return mp;
}

#ifdef CONFIG_MP_STAT_DEBUG
void *mp_update_stat_loop(void *arg)
{
	struct context *ctx;
	struct mem_pool *mp;
	struct msgbuf msg;
	int ret = 0;
	int num;

	mp = (struct mem_pool *)arg;
	if (mp == NULL) {
		printf("%s:%d, thread arg error!\n", __func__, __LINE__);
		return NULL;
	}

	//printf("%s:%d, begin to send mp stat by loop\n", __func__, __LINE__);

	for (;;) {
		if (mp->msg_queue == NULL)
			mp->msg_queue = get_msg_queue(MEM_POOL_STAT, MSQ_TYPE_FILE);

		if (mp->msg_queue == NULL) {
			num = gen_random(1, 1000);
			usleep(num * 1000);
		}

		if (mp->msg_queue == NULL)
			mp->msg_queue = create_msg_queue(MEM_POOL_STAT, MSQ_TYPE_FILE);

		if (mp->msg_queue == NULL) {
			sleep(1);
			continue;
		}

		ctx = get_current_context();
		if (ctx) {
			strncpy(msg.name, ctx->name, 15);
		} else {
			strncpy(msg.name, "unknown", 15);
		}

		msg.pid = getpid();
		msg.mtype = MSG_TYPE_MPSTAT;
		msg.length = sizeof(struct mem_pool_stat);
		memcpy(msg.buf, &mp->stat, sizeof(struct mem_pool_stat));

		if (ret == -1 && !updated)
			continue;

		ret = send_msg(mp->msg_queue, &msg);

		mp->mutex->ops->lock(mp->mutex);
		updated = false;
		mp->mutex->ops->unlock(mp->mutex);

		sleep(1);
	}

	return NULL;
}
#endif

struct purpose_node *find_purpose(struct mem_pool *mp, const char *purpose)
{
	struct purpose_node *p = NULL;

	if (purpose == NULL)
		return NULL;

	list_for_each_entry(p, &purposes, list) {
		if (strcmp(p->purpose, purpose) == 0) {
			return p;
		}
	}

	return NULL;
}

void add_purpose(struct mem_pool *mp, const char *purpose)
{
	struct purpose_node *p;

	p = find_purpose(mp, purpose);
	if (p == NULL) {
		p = purpose_node_create(purpose);
		p->count = 1;
		//list_add(&purposes, &p->list); // error!
		list_add(&p->list, &purposes);
		return;
	} else {
		p->count += 1;
	}
}

void sub_purpose(struct mem_pool *mp, const char *purpose)
{
	struct purpose_node *p;

	p = find_purpose(mp, purpose);
	if (p == NULL) {
		DEBUG((DEBUG_WARN, "sub purpose failed because cannot found "
			"purpose node: %s\n", purpose));
		return;
	}

	assert(p->count > 0);
	p->count -= 1;
	if (p->count == 0) {
		list_del(&p->list);
		free(p);
	}
}

/*
 * Alloc memory from mem pool
 * @mp: The mem pool pointer
 * @n: The size of memory to be allocated
 *
 * Return: The address of memory allocated
 */
void *mp_malloc(struct mem_pool *mp, const char *purpose, size_t n)
{
	struct mem_node *m_node = NULL, *p = NULL;

	if (mp == NULL || n == 0) {
		return NULL;
	}

	if (mp->mutex->ops->lock(mp->mutex) != 0) {
		perror("mp_malloc failed: cannot lock mem pool. ");
		return NULL;
	}

	list_for_each_entry(p, &mem_nodes, list) {
		if (p->is_using || p->size < n) {
			continue;
		}
		if (m_node == NULL) {
			m_node = p;
			continue;
		}
		if (p->size < m_node->size) {
			m_node = p;
		}
	}

	if (m_node == NULL) {
		m_node = mem_node_create(n);
		if (m_node == NULL) {
			mp->mutex->ops->unlock(mp->mutex);
			return NULL;
		}

		if (list_empty(&mem_nodes))
			mp->nodes = m_node;

		//list_add(&mem_nodes, &m_node->list); // error!
		list_add(&m_node->list, &mem_nodes);

		mp->stat.total_nodes += 1;
		mp->stat.total_size += n;
	} else {
		mp->stat.free_nodes -= 1;
	}

	m_node->is_using = 1;
	m_node->used_cnt += 1;
	m_node->purpose = purpose;

	updated = true;

	mp->stat.using_nodes += 1;

	add_purpose(mp, purpose);

	mp->mutex->ops->unlock(mp->mutex);

	bzero(m_node->address, n);

	//printf("purpose node count: %d\n", get_purpose_count(mp));

	return m_node->address;
}

/*
 * Realloc memmory
 * @mp: mem pool pointer
 * @addr: the address of memory to realloc
 * @n: the size of memory to realloc
 *
 * Return: the address reallocated
 */
void *mp_realloc(struct mem_pool *mp, void *addr, size_t n)
{
	struct mem_node *p = NULL;
	bool found = false;
	void *tmp;

	if (mp->mutex->ops->lock(mp->mutex) != 0) {
		perror("mp_realloc failed: cannot lock mem pool. ");
		return NULL;
	}

	list_for_each_entry(p, &mem_nodes, list) {
		if (p->address == addr) {
			found = true;
			break;
		}
	}

	mp->mutex->ops->unlock(mp->mutex);

	if (!found)
		return NULL;

	tmp = mp_malloc(mp, __func__, n);
	if (tmp == NULL)
		return NULL;

	if (n < p->size) {
		memcpy(tmp, addr, n);
	} else {
		memcpy(tmp, addr, p->size);
	}

	mp_free(mp, addr);

	return tmp;
}

/*
 * Free the memory from mem pool
 * @mp: mem pool pointer
 * @p: the address of memory from mem pool
 */
void mp_free(struct mem_pool *mp, void *p)
{
	struct mem_node *m_node = NULL;
#ifdef CONFIG_MP_FREE_DEBUG
	bool found = false;
#endif

	if (mp == NULL || p == NULL) {
		return ;
	}

	if (mp->mutex->ops->lock(mp->mutex) != 0) {
		perror("mp_free failed: cannot lock mem pool. ");
		return ;
	}

	list_for_each_entry(m_node, &mem_nodes, list) {
		if (m_node->address == p) {
			m_node->is_using = 0;
			//m_node->used_cnt += 1;
			mp->stat.free_nodes += 1;
			mp->stat.using_nodes -= 1;
#ifdef CONFIG_MP_FREE_DEBUG
			found = true;
#endif
			sub_purpose(mp, m_node->purpose);
			break;
		}
	}

#ifdef CONFIG_MP_FREE_DEBUG
	if (!found) {
		printf("mp_free failed, illegal address!\n");
	}
#endif

	mp->mutex->ops->unlock(mp->mutex);
}

/*
 * Destroy the mem pool
 * @mp: mem pool pointer
 */
void mp_destroy(struct mem_pool *mp)
{
	struct mem_node *m_node = NULL;

	if (mp == NULL) {
		return ;
	}

	list_for_each_entry(m_node, &mem_nodes, list) {
		if (m_node->is_using) {
			return ;
		}
	}

	list_for_each_entry(m_node, &mem_nodes, list)
		free(m_node);

	delete(pthread_mutex, mp->mutex);
}

/*
 * Get the node count of mem pool
 * @mp: mem pool pointer
 */
int mp_node_cnt(struct mem_pool *mp)
{
	int num = 0;
	struct mem_node *m_node = NULL;

	if (mp == NULL)
		return 0;

	list_for_each_entry(m_node, &mem_nodes, list)
		++num;

	return num;
}

/*
 * Get the global mem pool pointer
 *
 * Return: mem pool pointer
 */
struct mem_pool *mem_pool_construct(void)
{
	return get_singleton_mp();
}

/*
 * Destruct the mem pool
 * @mp: mem pool pointer
 */
void mem_pool_destruct(struct mem_pool *mp)
{
	mp_destroy(mp);
}

bool mp_valid(struct mem_pool *mp)
{
	return mp && mp->nodes;
}

/*
 * Get the size of mem pool
 * @mp: mem pool pointer
 *
 * Return: the size of mem pool
 */
int mp_size(struct mem_pool *mp)
{
	int sum =0;
	struct mem_node *m_node = NULL;

	if (mp == NULL) {
		return 0;
	}

	list_for_each_entry(m_node, &mem_nodes, list) {
		sum += m_node->size;
	}

	return sum;
}

void mp_stat(struct mem_pool *mp)
{
	if (!mp_valid(mp))
		return ;

	/* total cached memory */
	/* number of memory node */
	/* map of memory size and memory node */

	printf("mem_pool(%p): size -- %d, node -- %d\n", mp,
		mp_size(mp), mp_node_cnt(mp));

}

void get_mp_stat(struct mem_pool *mp, struct mem_pool_stat *stat)
{
	if (mp == NULL || stat == NULL)
		return;

	stat->total_size = mp->stat.total_size;
	stat->total_nodes = mp->stat.total_nodes;
	stat->using_nodes = mp->stat.using_nodes;
	stat->free_nodes = mp->stat.free_nodes;
}

int get_purpose_count(struct mem_pool *mp)
{
	int count = 0;
	struct purpose_node *p;

	if (mp == NULL)
		return 0;

	if (mp->mutex->ops->lock(mp->mutex) != 0) {
		perror("mp_free failed: cannot lock mem pool. ");
		return 0;
	}

	list_for_each_entry(p, &purposes, list)
		++count;

	mp->mutex->ops->unlock(mp->mutex);

	return count;
}

struct mem_pool_using_detail *get_mp_stat_detail(struct mem_pool *mp)
{
	int count = 0, i = 0;
	struct purpose_node *p;
	struct mem_pool_using_detail *detail = NULL;

	if (mp == NULL)
		return NULL;

	detail = mp_malloc(mp, "get_mp_stat_detail_struct", sizeof(struct mem_pool_using_detail));
	if (detail == NULL)
		goto done;

	count = get_purpose_count(mp);
	if (count <= 0) {
		printf("get purpose count not greater than zero!\n");
		goto done;
	}

	/*
	 * Number of pointer should be count + 1
	 */
	count += 1;
	detail->nodes = mp_malloc(mp, "get_mp_stat_detail_nodes", sizeof(struct using_node) * count);
	if (detail->nodes == NULL) {
		mp_free(mp, detail);
		detail = NULL;
		goto done;
	}

	if (mp->mutex->ops->lock(mp->mutex) != 0) {
		perror("mp_free failed: cannot lock mem pool. ");
		return 0;
	}
	list_for_each_entry(p, &purposes, list) {
		detail->nodes[i].purpose = p->purpose;
		detail->nodes[i].count = p->count;
		++i;
	}
	mp->mutex->ops->unlock(mp->mutex);

done:
	if (detail)
		detail->node_count = count;

	return detail;
}

int mem_pool_test(void)
{
	struct mem_pool *mp = mp_create();
	printf("node count: %d\n", mp_node_cnt(mp));

	char *str1 = (char *)mp_malloc(mp, "mem_pool_test1", 16);
	printf("node count: %d\n", mp_node_cnt(mp));
	strcpy(str1, "hello");

	char *str2 = (char *)mp_malloc(mp, "mem_pool_test2", 16);
	printf("node count: %d\n", mp_node_cnt(mp));
	strcpy(str2, ", ");

	char *str3 = (char *)mp_malloc(mp, "mem_pool_test3", 16);
	printf("node count: %d\n", mp_node_cnt(mp));
	strcpy(str3, "world");

	char *str4 = strcat(str1, str2);
	str4 = strcat(str4, str3);
	printf("str4: %s\n", str4);

	mp_free(mp, str1);
	printf("node count: %d\n", mp_node_cnt(mp));
	mp_free(mp, str2);
	printf("node count: %d\n", mp_node_cnt(mp));
	mp_free(mp, str3);
	printf("node count: %d\n", mp_node_cnt(mp));

	char *str5 = (char *)mp_malloc(mp, "mem_pool_test4", 16);
	printf("node count: %d\n", mp_node_cnt(mp));
	strcpy(str5, "hello, world");
	printf("str5: %s\n", str5);

	mp_free(mp, str5);
	mp_destroy(mp);

	return 0;
}
