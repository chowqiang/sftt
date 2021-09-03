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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "db.h"
#include "file.h"
#include "map.h"
#include "mem_pool.h"
#include "user.h"
#include "utils.h"

extern struct mem_pool *g_mp;

struct col_info user_cols[] = {
	{"uid", INTEGER},
	{"name", TEXT},
	{"passwd_md5", TEXT},
	{"home_dir", TEXT},
	{"create_time", INTEGER},
	{"update_time", INTEGER}
};

struct table_info user_table = {
	.cols = user_cols,
	.nr_cols = ARRAY_SIZE(user_cols),
};

char *db_search_pathes[] = {
	"/etc/sftt",
	"/root/sftt/db",
	"./db",
	".",
	NULL,
};

char *get_user_db_file(void)
{
	int i;
	char tmp[256];
	char *db_name = "user.db";

	for (i = 0; db_search_pathes[i] != NULL; ++i) {
		snprintf(tmp, 255, "%s/%s", db_search_pathes[i], db_name);
		if (file_existed(tmp))
			return __strdup(tmp);

	}

	return NULL;
}

struct user_base_info *find_user_base_by_name(char *name)
{
	int count = 0;
	struct map *data = NULL;
	struct db_connect *db_con;
	char *err_msg = NULL;
        char *value = NULL;
        char *db_file = NULL;
	char sql[1024];
	struct user_base_info *user_base = NULL;

	db_file = get_user_db_file();
	db_con = create_db_connect(db_file);
	assert(db_con != NULL);

	sprintf(sql, "select * from user where name='%s'", name);
	count = db_select(db_con, sql, &data, &err_msg);
	if (count < 1)
		return NULL;

	assert(count == 1);
	//printf("user count: %d\n", count);
	user_base = mp_malloc(g_mp, __func__, sizeof(struct user_base_info));
	assert(user_base != NULL);

	//show_keys(&data[0]);
	if (map_find(&data[0], str_equal, "uid", (void **)&value) == -1 || value == NULL) {
		printf("uid's value: %d\n", atoi(value));
		printf("cannot find uid\n");
		goto find_done;
	}
	user_base->uid = atoi(value);

	if (map_find(&data[0], str_equal, "name", (void **)&value) == -1 || value == NULL) {
		printf("cannot find name\n");
		goto find_done;
	}
	strcpy(user_base->name, value);

	if (map_find(&data[0], str_equal, "home_dir", (void **)&value) == -1) {
		printf("cannot find home dir\n");
		goto find_done;
	}
	if (value)
		strcpy(user_base->home_dir, value);

	if (map_find(&data[0], str_equal, "create_time", (void **)&value) == -1) {
		printf("cannot find create_time\n");
		goto find_done;
	}
	if (value)
		user_base->create_time = atoi(value);

	if (map_find(&data[0], str_equal, "update_time", (void **)&value) == -1) {
		printf("cannot find update_time\n");
		goto find_done;
	}
	if (value)
		user_base->update_time = atoi(value);

find_done:
	map_destroy(&data[0]);

	return user_base;
}

struct user_auth_info *find_user_auth_by_name(char *name)
{
	int i, count;
	struct map *data;
	struct db_connect *db_con;
	char *err_msg, *value, *db_file;
	char sql[1024];
	struct user_auth_info *user_auth = NULL;

	db_file = get_user_db_file();
	db_con = create_db_connect(db_file);
	assert(db_con != NULL);

	sprintf(sql, "select * from user where name='%s'", name);
	count = db_select(db_con, sql, &data, &err_msg);
	if (count < 1)
		return NULL;
	assert(count == 1);
	//printf("user count: %d\n", count);

	user_auth = mp_malloc(g_mp, __func__, sizeof(struct user_auth_info));
	assert(user_auth != NULL);

	if (map_find(&data[0], str_equal, "name", (void **)&value) == -1 || value == NULL) {
		printf("cannot find uid\n");
		goto find_done;
	}
	strcpy(user_auth->name, value);

	if (map_find(&data[0], str_equal, "passwd_md5", (void **)&value) == -1 || value == NULL) {
		printf("cannot find name\n");
		goto find_done;
	}
	strcpy(user_auth->passwd_md5, value);

find_done:
	map_destroy(&data[0]);

	return user_auth;
}

int user_add(char *name, char *passwd_md5)
{
	int count;
	int uid;
	char *db_file;
	struct db_connect *db_con;
	char sql[1024];
	char *err_msg;

	count = get_user_count();
	if (count == -1) {
		return -1;
	}

	db_file = get_user_db_file();
	db_con = create_db_connect(db_file);
	assert(db_con != NULL);

	uid = count + 1;
	snprintf(sql, 1023, "insert into user values(%d, '%s', '%s')", uid, name, passwd_md5);
	db_insert(db_con, sql, &err_msg);

	return 0;
}

int get_user_count(void)
{
	char *db_file;
	struct db_connect *db_con;
	char *sql = "select count(*) from user";
	char *err_msg;

	db_file = get_user_db_file();
	db_con = create_db_connect(db_file);
	assert(db_con != NULL);

	return db_select_count(db_con, sql, &err_msg);
}

int update_user_base_info(char *name, char *key, char *value)
{
	char *db_file;
	struct db_connect *db_con;
	char buf[1024];
	char sql[1024];
	char *err_msg;
	enum col_type type;
	const char *fmt;

	db_file = get_user_db_file();
	db_con = create_db_connect(db_file);
	assert(db_con != NULL);

	type = get_col_type(&user_table, key);
	assert(type != UNKNOWN_COL_TYPE);

	fmt = col_fmt(type);

	bzero(buf, 1024);
	strcat(buf, "update user set %s='");
	strcat(buf, fmt);
	strcat(buf, "', update_time=%d where name='%s'");

	snprintf(sql, 1023, buf, key, col_value(type, value),
			get_ts(), name);

	return db_update(db_con, sql, &err_msg);
}
