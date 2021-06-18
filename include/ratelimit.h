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

#ifndef _RATELIMIT_H_
#define _RATELIMIT_H_

#include <stdbool.h>
#include <sys/time.h>
#include "lock.h"

struct ratelimit_state {
	struct pthread_mutex *mutex;
	int     interval;
	int     burst;
	int     used;
	int     missed;
	time_t  last;
};

struct ratelimit_state *ratelimit_state_construct(int interval,
	int burst);
void ratelimit_state_destruct(struct ratelimit_state *ptr);
bool ratelimit_try_inc(struct ratelimit_state *rs);

#endif
