#include <assert.h>
#include <stdio.h>
#include "base.h"
#include "lock.h"
#include "mem_pool.h"
#include "ratelimit.h"
#include "utils.h"

extern struct mem_pool *g_mp;

struct ratelimit_state *ratelimit_state_construct(int interval,
	int burst)
{
	struct ratelimit_state *rs;

	rs = mp_malloc(g_mp, sizeof(struct ratelimit_state));
	assert(rs != NULL);
	
	rs->interval = interval;
	rs->burst = burst;
	rs->last = get_ts();

	rs->used = 0;
	rs->missed = 0;
	
	rs->mutex = new(pthread_mutex);
	assert(rs->mutex);

	return rs;
}

void ratelimit_state_destruct(struct ratelimit_state *ptr)
{
	mp_free(g_mp, ptr);
}

bool ratelimit_try_inc(struct ratelimit_state *rs)
{
	if (rs->mutex->ops->lock(rs->mutex)) {
		printf("lock ratelimit failed!\n");
		return false;
	}

	time_t now = get_ts();	
	bool ret = true;

	if ((now - rs->last) >= rs->interval) {
		rs->used *= (rs->interval * 1.0 / (now - rs->last));
		rs->last = now - rs->interval;
	}
	
	if (rs->used >= rs->burst) {
		ret = false;
	}

	if (ret)
		rs->used++;
	else
		rs->missed++;

	rs->mutex->ops->unlock(rs->mutex);

	return ret;
}
