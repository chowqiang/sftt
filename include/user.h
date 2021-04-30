#ifndef _USER_H_
#define _USER_H_

#include <stdbool.h>
#include <stdint.h>
#include "md5.h"
#include "req_rsp.h"

struct user_info {
	uint64_t uid;
	char name[USER_NAME_MAX_LEN + 1];
	//char passwd[PASSWD_MAX_LEN];
	//char passwd_md5[PASSWD_MAX_LEN];
};

bool user_add(struct user_info ui);

#endif
