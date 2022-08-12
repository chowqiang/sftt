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

#include <stdio.h>
#include <string.h>
#include "base.h"
#include "mem_pool.h"

struct mp_thread_info {
	struct mem_pool *mp;
	int idx;
};

void *test_mp_thread(void *info)
{
	struct mp_thread_info *ti = (struct mp_thread_info *)info;
	char *buf;
	printf("I'm thread %d\n", ti->idx);
	if (ti->idx % 3) {
		buf = mp_malloc(ti->mp, __func__, 32);
	}
	mp_stat(ti->mp);
	if (ti->idx % 3 == 1) {
		mp_free(ti->mp, buf);
	}

	return NULL;
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

int main(int argc, char *argv[])
{
	struct mem_pool *mp = new(mem_pool);
	pthread_t thread_ids[8];
	struct mp_thread_info thread_infos[8];
	void *pret = NULL;
	int ret = 0, i = 0;

	for (i = 0; i < 8; ++i) {
		thread_infos[i].mp = mp;
		thread_infos[i].idx = i;
		ret = pthread_create(&thread_ids[i], NULL, test_mp_thread, &thread_infos[i]);
		if (ret) {
			printf("create pthread failed! idx=%d\n", i);
		}
	}


	for (i = 0; i < 8; ++i) {
		pthread_join(thread_ids[i], &pret);
	}

	mp_stat(mp);

	delete(mem_pool, mp);

	return 0;
}
