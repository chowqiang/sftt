#ifndef _USER_H_
#define _USER_H_

#include <stdbool.h>
#include <stdint.h>
#include "md5.h"
#include "req_rsp.h"

struct user_base_info {
	uint64_t uid;
	char name[USER_NAME_MAX_LEN + 1];
};

struct user_auth_info {
	uint64_t uid;
	char passwd_md5[PASSWD_MAX_LEN];
};

int user_add(char *name, char *passwd_md5);

struct user_base_info *find_user_base_by_name(char *name);

struct user_auth_info *find_user_auth_by_id(uint64_t uid);

char *get_user_db_file(void);

#endif
