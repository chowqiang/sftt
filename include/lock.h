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

#ifndef _LOCK_H_
#define _LOCK_H_

#include <pthread.h>

struct pthread_mutex {
	pthread_mutex_t mutex;
	struct pthread_mutex_ops *ops;
};

struct pthread_mutex_ops {
	int (*init)(struct pthread_mutex *mutex);
	int (*lock)(struct pthread_mutex *mutex);
	int (*unlock)(struct pthread_mutex *mutex);
	int (*destroy)(struct pthread_mutex *mutex);
};

int sftt_pthread_mutex_init(struct pthread_mutex *mutex);
int sftt_pthread_mutex_lock(struct pthread_mutex *mutex);
int sftt_pthread_mutex_unlock(struct pthread_mutex *mutex);
int sftt_pthread_mutex_destroy(struct pthread_mutex *mutex);

struct pthread_mutex *pthread_mutex_construct(void);
void pthread_mutex_destruct(struct pthread_mutex *ptr);

#endif
