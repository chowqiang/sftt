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

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef CONFIG_THREAD_POOL_MAX_SIZE
#define THREAD_POOL_MAX_SIZE CONFIG_THREAD_POOL_MAX_SIZE
#else
#define THREAD_POOL_MAX_SIZE 32
#endif

/**
 * Definition of a remote launch function.
 */
typedef int (thread_function_t)(void *);

/**
 * State of an lcore.
 */
enum thread_state_t {
        WAIT,       /**< waiting a new command */
        RUNNING,    /**< executing command */
        FINISHED,   /**< command executed */
};

struct thread_instance {
	pthread_t thread_id;       /**< pthread identifier */
	int pipe_main2worker[2];   /**< communication pipe with main */
	int pipe_worker2main[2];   /**< communication pipe with main */

	thread_function_t * volatile f; /**< function to call */
	void * volatile arg;       /**< argument of function */
	volatile int ret;          /**< return value of function */

	volatile enum thread_state_t state; /**< thread state */
	int thread_index;            /**< relative index, starting from 0 */
	uint8_t thread_role;         /**< role of thread eg: OFF, RTE, SERVICE */
};

struct thread_pool {
	int size;
	bool initialized;
	struct thread_instance *threads;
};

struct thread_pool *create_thread_pool(int size);

void destroy_thread_pool(struct thread_pool *tp);

int launch_thread_in_pool(struct thread_pool *tp, int tid,
		int (*f)(void *), void *args);

int launch_one_thread(struct thread_instance *ti,
		int (*f)(void *), void *args);
#endif
