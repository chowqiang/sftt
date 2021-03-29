#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mem_pool.h"

mem_pool_t *g_mp = NULL;

static void __attribute__((constructor)) mem_pool_init(void)
{
	get_singleton_mp();	
}

mem_pool_t *get_singleton_mp() {
	if (g_mp) {
		return g_mp;
	}

	g_mp = mp_create();

	return g_mp;
}

mem_node_t *mem_node_t_create(size_t size) {
	mem_node_t *p = (mem_node_t *)malloc(sizeof(mem_node_t));
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

	return p;
}

void mem_node_t_free(void *data) {
	if (data == NULL) {
		return ;
	}
	mem_node_t *node = (mem_node_t *)data;
	if (!node->is_using && node->address) {
		free(node->address);
	}
}

mem_pool_t *mp_create(void) {
	mem_pool_t *mp = (mem_pool_t *)malloc(sizeof(mem_pool_t));
	if (mp == NULL) {
		return NULL;
	}
	mp->list = dlist_create(mem_node_t_free);

	return mp;
}

int mp_init(mem_pool_t *mp) {
	if (mp == NULL) {
		return -1;
	}
	mp->list = dlist_create(mem_node_t_free);

	return 0;
}

void *mp_malloc(mem_pool_t *mp, size_t n) {
	if (mp == NULL || mp->list == NULL || n == 0) {
		return NULL;
	}

	mem_node_t *m_node = NULL, *p = NULL;
	dlist_node *l_node = NULL;
	dlist_for_each(mp->list, l_node) {
		p = (mem_node_t *)l_node->data;
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
		m_node = mem_node_t_create(n);
		if (m_node == NULL) {
			return NULL;
		}
		dlist_append(mp->list, m_node);
	}

	m_node->is_using = 1;
	m_node->used_cnt += 1;

	return m_node->address;
}

void mp_free(mem_pool_t *mp, void *p) {
	if (mp == NULL || mp->list == NULL || p == NULL) {
		return ;
	}
	mem_node_t *m_node = NULL;
	dlist_node *l_node = NULL;
	dlist_for_each(mp->list, l_node) {
		m_node = (mem_node_t *)l_node->data;
		if (m_node->address == p) {
			m_node->is_using = 0;
			m_node->used_cnt += 1;
			break;
		}
	}
}

void mp_destroy(mem_pool_t *mp) {
	if (mp == NULL || mp->list == NULL) {
		return ;
	}
	mem_node_t *m_node = NULL;
	dlist_node *l_node = NULL;
	dlist_for_each(mp->list, l_node) {
		m_node = (mem_node_t *)l_node->data;
		if (m_node->is_using) {
			return ;
		}
	}

	dlist_destroy(mp->list);
}

int mp_node_cnt(mem_pool_t *mp) {
	if (mp == NULL || mp->list == NULL) {
		return 0;
	}

	return dlist_size(mp->list);
}

mem_pool_t *mem_pool_t_contruct(void)
{
	return NULL;
}

void mem_pool_t_deconstruct(mem_pool_t *ptr)
{

}

#if 0
int main(void) {
	mem_pool_t *mp = mp_create();
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
#endif
