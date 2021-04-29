#include <assert.h>
#include <stdlib.h>
#include "lock.h"

struct pthread_mutex_ops pm_ops = {
	.init = sftt_pthread_mutex_init,
	.lock = sftt_pthread_mutex_lock,
	.unlock = sftt_pthread_mutex_unlock,
 	.destroy = sftt_pthread_mutex_destroy,	
};

struct pthread_mutex *pthread_mutex_construct(void)
{
	struct pthread_mutex *pm;
	
	/*
	 * here cannot use mem_pool to alloc memory,
	 * because mem_pool depend on pthred_mutex.
	 */
	pm = malloc(sizeof(struct pthread_mutex));
	assert(pm != NULL);

	pm->ops = &pm_ops;
	pm->ops->init(pm);

	return pm;
}

void pthread_mutex_destruct(struct pthread_mutex *ptr)
{
	ptr->ops->destroy(ptr);
	free(ptr);
}

int sftt_pthread_mutex_init(struct pthread_mutex *mutex)
{
	return pthread_mutex_init(&mutex->mutex, NULL);
}

int sftt_pthread_mutex_lock(struct pthread_mutex *mutex)
{
	return pthread_mutex_lock(&mutex->mutex);
}

int sftt_pthread_mutex_unlock(struct pthread_mutex *mutex)
{
	return pthread_mutex_unlock(&mutex->mutex);	
}

int sftt_pthread_mutex_destroy(struct pthread_mutex *mutex)
{
	return pthread_mutex_destroy(&mutex->mutex);
}

