#ifndef _USER_H_
#define _USER_H_

#include <stdbool.h>
#include <stdint.h>
#include "md5.h"
#include "req_rsp.h"

enum user_validate_status {
	UVS_NTFD,
	UVS_INVALID,
	UVS_MISSHOME,
	UVS_BLOCK,
	UVS_PASS
};

struct user_base_info {
	uint64_t uid;
	char name[USER_NAME_MAX_LEN];
	char home_dir[DIR_PATH_MAX_LEN];
	int create_time;
	int update_time;
};

struct user_auth_info {
	char name[USER_NAME_MAX_LEN];
	char passwd_md5[PASSWD_MD5_LEN];
};

int user_add(char *name, char *passwd_md5);

struct user_base_info *find_user_base_by_name(char *name);

struct user_auth_info *find_user_auth_by_name(char *name);

char *get_user_db_file(void);

int get_user_count(void);

int update_user_base_info(char *name, char *key, char *value);

#endif
