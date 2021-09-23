/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _USER_H_
#define _USER_H_

#include <stdbool.h>
#include <stdint.h>
#include "md5.h"
#include "req_resp.h"

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
