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
#include "base.h"
#include "mem_pool.h"

/*
 * The global mem pool pointer
 */
struct mem_pool *g_mp = NULL;

static LIST_HEAD(mem_nodes);

/*
 * Make the g_mp available
 */
static void __attribute__((constructor)) mem_pool_init(void)
{
	get_singleton_mp();	
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
	if (data == NULL) {
		return ;
	}
	struct mem_node *node = (struct mem_node *)data;
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
	struct mem_pool *mp = (struct mem_pool *)malloc(sizeof(struct mem_pool));
	if (mp == NULL) {
		return NULL;
	}
	mp->nodes = NULL;

	mp->mutex = new(pthread_mutex);
	assert(mp->mutex != NULL);

	return mp;
}

/*
 * Alloc memory from mem pool
 * @mp: The mem pool pointer
 * @n: The size of memory to be allocated
 *
 * Return: The address of memory allocated
 */
void *mp_malloc(struct mem_pool *mp, size_t n)
{
	if (mp == NULL || n == 0) {
		return NULL;
	}

	struct mem_node *m_node = NULL, *p = NULL;
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
			return NULL;
		}

		if (list_empty(&mem_nodes))
			mp->nodes = m_node;

		list_add(&mem_nodes, &m_node->list);
	}

	m_node->is_using = 1;
	m_node->used_cnt += 1;
	mp->mutex->ops->unlock(mp->mutex);

	bzero(m_node->address, n);

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
	if (!found)
		return NULL;

	tmp = mp_malloc(mp, n);
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
	if (mp == NULL || p == NULL) {
		return ;
	}
	struct mem_node *m_node = NULL;
	if (mp->mutex->ops->lock(mp->mutex) != 0) {
		perror("mp_free failed: cannot lock mem pool. ");
		return ;
	}
	list_for_each_entry(m_node, &mem_nodes, list) {
		if (m_node->address == p) {
			m_node->is_using = 0;
			m_node->used_cnt += 1;
			break;
		}
	}
	mp->mutex->ops->unlock(mp->mutex);
}

/*
 * Destroy the mem pool
 * @mp: mem pool pointer
 */
void mp_destroy(struct mem_pool *mp)
{
	if (mp == NULL) {
		return ;
	}

	struct mem_node *m_node = NULL;
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

	if (mp == NULL)
		return 0;

	struct mem_node *m_node = NULL;
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
	if (mp == NULL) {
		return 0;
	}

	int sum =0;
	struct mem_node *m_node = NULL;
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

int mem_pool_test(void)
{
	struct mem_pool *mp = mp_create();
	printf("node count: %d\n", mp_node_cnt(mp));

	char *str1 = (char *)mp_malloc(mp, 16);
	printf("node count: %d\n", mp_node_cnt(mp));
	strcpy(str1, "hello");

	char *str2 = (char *)mp_malloc(mp, 16);
	printf("node count: %d\n", mp_node_cnt(mp));
	strcpy(str2, ", ");

	char *str3 = (char *)mp_malloc(mp, 16);
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

	char *str5 = (char *)mp_malloc(mp, 16);
	printf("node count: %d\n", mp_node_cnt(mp));
	strcpy(str5, "hello, world");
	printf("str5: %s\n", str5);

	mp_free(mp, str5);
	mp_destroy(mp);

	return 0;
}
