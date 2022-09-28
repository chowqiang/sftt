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

#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <unistd.h>
#include "debug.h"
#include "mem_pool.h"
#include "thread_pool.h"

extern struct mem_pool *g_mp;
void
worker_thread_wait_command(struct thread_instance *thread)
{
	int m2w;
	char c;
	int n;

	m2w = thread->pipe_main2worker[0];
	do {
		n = read(m2w, &c, 1);
	} while (n < 0 && errno == EINTR);

	if (n <= 0) {
		DEBUG((DEBUG_ERROR, "cannot read on configuration pipe\n"));
		abort();
	}
}

void
worker_thread_ack_command(struct thread_instance *thread)
{
        char c = 0;
        int w2m;
        int n;

        w2m = thread->pipe_worker2main[1];
        do {
                n = write(w2m, &c, 1);
        } while (n == 0 || (n < 0 && errno == EINTR));
        if (n < 0) {
                DEBUG((DEBUG_ERROR, "cannot write on configuration pipe\n"));
		abort();
	}
}

void
wake_worker_thread(struct thread_instance *ti)
{
        int m2w = ti->pipe_main2worker[1];
        int w2m = ti->pipe_worker2main[0];
        char c = 0;
        int n;

        do {
                n = write(m2w, &c, 1);
        } while (n == 0 || (n < 0 && errno == EINTR));
        if (n < 0) {
                DEBUG((DEBUG_ERROR, "cannot write on configuration pipe\n"));
		abort();
	}

        do {
                n = read(w2m, &c, 1);
        } while (n < 0 && errno == EINTR);
        if (n <= 0) {
                DEBUG((DEBUG_ERROR, "cannot read on configuration pipe\n"));
		abort();
	}
}

int
wait_worker_thread(struct thread_instance *ti)
{
        while (__atomic_load_n(&ti->state,
                        __ATOMIC_ACQUIRE) != WAIT)
                usleep(1);

        return ti->ret;
}

void
stop_worker_thread(struct thread_instance *ti)
{
	pthread_cancel(ti->thread_id);
	pthread_join(ti->thread_id, NULL);
}

void *worker_thread_loop(void *arg)
{
	struct thread_instance *thread = arg;
	int ret;

	while (1) {
		thread_function_t *f;
		void *fct_arg;

		worker_thread_wait_command(thread);

		/* Set the state to 'RUNNING'. Use release order
		 * since 'state' variable is used as the guard variable.
		 */
		__atomic_store_n(&thread->state, RUNNING,
			__ATOMIC_RELEASE);

		worker_thread_ack_command(thread);

		/* Load 'f' with acquire order to ensure that
		 * the memory operations from the main thread
		 * are accessed only after update to 'f' is visible.
		 * Wait till the update to 'f' is visible to the worker.
		 */
		while ((f = __atomic_load_n(&thread->f,
				__ATOMIC_ACQUIRE)) == NULL)
			usleep(1);

		/* call the function and store the return value */
		fct_arg = thread->arg;
		ret = f(fct_arg);
		thread->ret = ret;
		thread->f = NULL;
		thread->arg = NULL;

		/* Store the state with release order to ensure that
		 * the memory operations from the worker thread
		 * are completed before the state is updated.
		 * Use 'state' as the guard variable.
		 */
		__atomic_store_n(&thread->state, WAIT,
			__ATOMIC_RELEASE);
	}
}

int create_worker_thread(struct thread_instance *thread)
{
	/*
	 * create communication pipes between main thread
	 * and children
	 */
	if (pipe(thread->pipe_main2worker) < 0) {
		DEBUG((DEBUG_ERROR, "Cannot create pipe\n"));
		return -1;
	}

	if ((pipe(thread->pipe_worker2main) < 0)) {
		DEBUG((DEBUG_ERROR, "Cannot create pipe\n"));
		return -1;
	}

	thread->state = WAIT;

	return pthread_create(&thread->thread_id, NULL, worker_thread_loop, thread);
}

int init_thread_pool(struct thread_pool *tp)
{
	int i, ret = 0;

	if (tp == NULL)
		return -1;

	if (tp->initialized)
		return 0;

	for (i = 0; i < tp->size; ++i) {
		tp->threads[i].thread_index = i;
		ret = create_worker_thread(&tp->threads[i]);
		if (ret)
			break;
	}

	return ret;
}

struct thread_pool *create_thread_pool(int size)
{
	struct thread_pool *tp = NULL;
	int ret;

	if (size > THREAD_POOL_MAX_SIZE) {
		DEBUG((DEBUG_ERROR, "thread pool size cannot exceed %d\n", THREAD_POOL_MAX_SIZE));
		return NULL;
	}

	tp = mp_malloc(g_mp, __func__, sizeof(struct thread_pool));
	if (tp == NULL) {
		DEBUG((DEBUG_ERROR, "cannot alloc memory for thread pool\n"));
		return NULL;
	}

	tp->threads = mp_malloc(g_mp, __func__, sizeof(struct thread_instance) * size);
	if (tp->threads == NULL) {
		DEBUG((DEBUG_ERROR, "cannot alloc memory for threads\n"));
		goto free_tp;
		return NULL;
	}
	tp->size = size;
	tp->initialized = false;
	ret = init_thread_pool(tp);
	if (ret < 0)
		goto free_tp;
	tp->initialized = true;

	return tp;
free_tp:
	mp_free(g_mp, tp);
	return NULL;
}

void destroy_thread_pool(struct thread_pool *tp)
{
	int i = 0;

	if (!tp->initialized)
		return;

	for (i = 0; i < tp->size; ++i) {
		wait_worker_thread(&tp->threads[i]);
		stop_worker_thread(&tp->threads[i]);
	}

	tp->initialized = false;
	tp->size = 0;
	tp->threads = NULL;

	mp_free(g_mp, tp->threads);
	mp_free(g_mp, tp);
}

int launch_one_thread(struct thread_instance *ti,
		int (*f)(void *), void *args)
{
        int rc = -EBUSY;

        /* Check if the worker is in 'WAIT' state. Use acquire order
         * since 'state' variable is used as the guard variable.
         */
        if (__atomic_load_n(&ti->state,
                        __ATOMIC_ACQUIRE) != WAIT)
                goto finish;

        ti->arg = args;
        /* Ensure that all the memory operations are completed
         * before the worker thread starts running the function.
         * Use worker thread function as the guard variable.
         */
        __atomic_store_n(&ti->f, f, __ATOMIC_RELEASE);

        wake_worker_thread(ti);
        rc = 0;
finish:
        return rc;
}

int get_one_idle_thread(struct thread_pool *tp, int *tid)
{
	int i = 0;

	if (tp == NULL || tid == NULL)
		return -1;

	if (!tp->initialized)
		return -1;

	for (i = 0; i < tp->size; ++i) {
		if (__atomic_load_n(&tp->threads[i].state,
                        __ATOMIC_ACQUIRE) == WAIT) {
			*tid = i;
			return 0;
		}
	}

	return -1;
}

int launch_thread_in_pool(struct thread_pool *tp, int tid,
		int (*f)(void *), void *args)
{
	int ret;

	if (tid == THREAD_INDEX_ANY) {
		ret = get_one_idle_thread(tp, &tid);
		if (ret)
			return -1;
	}

	return launch_one_thread(&tp->threads[tid], f, args);
}
