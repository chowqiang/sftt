#ifndef _TRACE_H_
#define _TRACE_H_

#include "user.h"

#define TRACE_ID_LEN 16	

struct trace_info {
	char id[TRACE_ID_LEN + 1];
	struct user_base_info *user;
	struct trace_info_ops *ops;
};

struct trace_info_ops {
	int (*set_user)(struct trace_info *trace, struct user_base_info *user);
	struct sds *(*to_str)(struct trace_info *trace);
};

int trace_info_set_user(struct trace_info *trace, struct user_base_info *user);
struct sds *trace_info_to_str(struct trace_info *trace);

#endif
