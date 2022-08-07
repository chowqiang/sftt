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

#include <stdbool.h>
#include <stdio.h>
#include "thread_pool.h"

#define FUNC_GET_SUM(id) get_sum_##id
#define DEFINE_FUNC_GET_SUM(id) 			\
       int FUNC_GET_SUM(id)(void *arg);			\
       int FUNC_GET_SUM(id)(void *arg) 			\
       {						\
	        int i = 0;				\
		int sum = 0;				\
	        int num = (int)(unsigned long)arg;	\
							\
		for (i = 1; i <= num; ++i) {		\
			sum += i;			\
		}					\
							\
		return sum;				\
       }

DEFINE_FUNC_GET_SUM(0)
DEFINE_FUNC_GET_SUM(1)
DEFINE_FUNC_GET_SUM(2)
DEFINE_FUNC_GET_SUM(3)
DEFINE_FUNC_GET_SUM(4)
DEFINE_FUNC_GET_SUM(5)
DEFINE_FUNC_GET_SUM(6)
DEFINE_FUNC_GET_SUM(7)
DEFINE_FUNC_GET_SUM(8)
DEFINE_FUNC_GET_SUM(9)
DEFINE_FUNC_GET_SUM(10)
DEFINE_FUNC_GET_SUM(11)
DEFINE_FUNC_GET_SUM(12)
DEFINE_FUNC_GET_SUM(13)
DEFINE_FUNC_GET_SUM(14)
DEFINE_FUNC_GET_SUM(15)

static int test_basic(void)
{
	struct thread_pool *tp = 0;
	bool has_error = false;
	int num = 16;
	int i = 0;
	int ret = 0;

	tp = create_thread_pool(num);
	if (tp == NULL) {
		return -1;
	}

	launch_thread_in_pool(tp, 0, FUNC_GET_SUM(0), (void *)100);
	launch_thread_in_pool(tp, 1, FUNC_GET_SUM(1), (void *)200);
	launch_thread_in_pool(tp, 2, FUNC_GET_SUM(2), (void *)300);
	launch_thread_in_pool(tp, 3, FUNC_GET_SUM(3), (void *)400);
	launch_thread_in_pool(tp, 4, FUNC_GET_SUM(4), (void *)500);
	launch_thread_in_pool(tp, 5, FUNC_GET_SUM(5), (void *)600);
	launch_thread_in_pool(tp, 6, FUNC_GET_SUM(6), (void *)700);
	launch_thread_in_pool(tp, 7, FUNC_GET_SUM(7), (void *)800);
	launch_thread_in_pool(tp, 8, FUNC_GET_SUM(8), (void *)900);
	launch_thread_in_pool(tp, 9, FUNC_GET_SUM(9), (void *)1000);
	launch_thread_in_pool(tp, 10, FUNC_GET_SUM(10), (void *)1100);
	launch_thread_in_pool(tp, 11, FUNC_GET_SUM(11), (void *)1200);
	launch_thread_in_pool(tp, 12, FUNC_GET_SUM(12), (void *)1300);
	launch_thread_in_pool(tp, 13, FUNC_GET_SUM(13), (void *)1400);
	launch_thread_in_pool(tp, 14, FUNC_GET_SUM(14), (void *)1500);
	launch_thread_in_pool(tp, 15, FUNC_GET_SUM(15), (void *)1600);

	for (i = 0; i < num; ++i) {
		ret = wait_worker_thread(&tp->threads[i]);
		if (ret != FUNC_GET_SUM(1)((void *)(unsigned long)((i + 1) * 100))) {
			printf("thread %i calculated error! ret = %d\n", i, ret);
			has_error = true;
		} else {
			printf("thread %i calculated correct! ret = %d\n", i, ret);
		}
	}

	destroy_thread_pool(tp);

	if (has_error)
		return -1;

	return 0;
}

int main(void)
{
	return test_basic();
}
