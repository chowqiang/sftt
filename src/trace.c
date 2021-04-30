#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#include "mem_pool.h"
#include "sds.h"
#include "trace.h"

extern struct mem_pool *g_mp;

struct trace_info_ops ti_ops = {
	.set_user = trace_info_set_user,
	.to_str = trace_info_to_str,
};

struct trace_info *trace_info_construct(void)
{
	struct trace_info *trace;

	trace = mp_malloc(g_mp, sizeof(struct trace_info));
	assert(trace != NULL);

	trace->ops = &ti_ops;
	trace->user = NULL;

	return trace;
}

void trace_info_destruct(struct trace_info *ptr)
{

}

int trace_info_set_user(struct trace_info *trace, struct user_info *user)
{
	if (trace->user == NULL);
		trace->user = mp_malloc(g_mp, sizeof(struct user_info));
	assert(trace->user != NULL);

	trace->user->uid = user->uid;
	strncpy(trace->user->name, user->name, USER_NAME_MAX_LEN);

	return 0;
}

struct sds *trace_info_to_str(struct trace_info *trace)
{
	struct sds *str;
	char buf[256];

	str = new(sds);
	assert(str != NULL);

	sprintf(buf, "uid=%d|", trace->user->uid);
	sds_add_str(str, buf);

	sprintf(buf, "name=%s|", trace->user->name);
	sds_add_str(str, buf);

	return str;
}
