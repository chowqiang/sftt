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
