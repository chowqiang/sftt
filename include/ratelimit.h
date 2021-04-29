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
