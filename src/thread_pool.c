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

#include "thread_pool.h"

struct thread_pool *create_thread_pool(int size)
{
	return NULL;
}

void destroy_thread_pool(struct thread_pool *tp)
{

}

int launch_thread_in_pool(struct thread_pool *tp, int tid,
		int (*f)(void *), void *args)
{
	return 0;
}

int launch_one_thread(struct thread_instance *ti,
		int (*f)(void *), void *args)
{
	return 0;
}
