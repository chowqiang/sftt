#ifndef _TRACE_H_
#define _TRACE_H_

#define TRACE_ID_LEN 16	

struct trace_info {
	char id[TRACE_ID_LEN + 1];
	struct user_info *user;
};


#endif
