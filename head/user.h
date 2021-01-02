#ifndef _USER_H_
#define _USER_H_

#include <stdbool.h>
#include <stdint.h>
#include "config.h"
#include "md5.h"

typedef struct user_info {
	uint64_t uid;
	char name[USER_NAME_MAX_LEN];
	char passwd[USER_PASSWD_MAX_LEN];
	char passwd_md5[MD5_LEN + 1];
} user_info;

bool user_add(user_info ui);

#endif
