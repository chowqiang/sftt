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
