#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "db.h"
#include "file.h"
#include "map.h"
#include "mem_pool.h"
#include "user.h"

extern struct mem_pool *g_mp;

struct user_base_info users[] = {
	{123456, "root"},
	{19910930, "zhoumin"},
	{19921112, "yanan"},
	{-1, ""}
};

char *db_search_pathes[] = {
	"/etc/sftt",
	"./db",
	".",
	NULL,
};

struct user_auth_info auths = {

};

char *get_user_db_file(void)
{
	int i;
	char tmp[256];
	char *db_name = "user.db";

	for (i = 0; db_search_pathes[i] != NULL; ++i) {
		snprintf(tmp, 255, "%s/%s", db_search_pathes[i], db_name);
		if (file_is_existed(tmp))
			return strdup(tmp);

	}

	return NULL;
}

struct user_base_info *find_user_base_by_name(char *name)
{
	int i, count;
	struct map *data;
	struct db_connect *db_con;
	char *err_msg, *value, *db_file;
	char sql[1024];
	struct user_base_info *user_base;

	db_file = get_user_db_file();
	db_con = create_db_connect(db_file);
	assert(db_con != NULL);

	sprintf(sql, "select * from user where name='%s'", name);
	count = db_select(db_con, sql, &data, &err_msg);
	if (count < 1)
		return NULL;

	assert(count == 1);
	printf("user count: %d\n", count);
	user_base = mp_malloc(g_mp, sizeof(struct user_base_info));
	assert(user_base != NULL);

	if (map_find(&data[i], str_equal, "uid", (void **)&value) == -1 || value == NULL) {
		printf("cannot find uid\n");
		return NULL;
	}
	user_base->uid = atoi(value);

	if (map_find(&data[i], str_equal, "name", (void **)&value) == -1 || value == NULL) {
		printf("cannot find name\n");
		return NULL;
	}
	strcpy(user_base->name, value);

	return user_base;
}

struct user_auth_info *find_user_auth_by_name(char *name)
{
	int i, count;
	struct map *data;
	struct db_connect *db_con;
	char *err_msg, *value, *db_file;
	char sql[1024];
	struct user_auth_info *user_auth;

	db_file = get_user_db_file();
	db_con = create_db_connect(db_file);
	assert(db_con != NULL);

	sprintf(sql, "select * from user where name='%s'", name);
	count = db_select(db_con, sql, &data, &err_msg);
	if (count < 1)
		return NULL;

	assert(count == 1);
	printf("user count: %d\n", count);
	user_auth = mp_malloc(g_mp, sizeof(struct user_auth_info));
	assert(user_auth != NULL);

	if (map_find(&data[i], str_equal, "name", (void **)&value) == -1 || value == NULL) {
		printf("cannot find uid\n");
		return NULL;
	}
	strcpy(user_auth->name, value);

	if (map_find(&data[i], str_equal, "passwd_md5", (void **)&value) == -1 || value == NULL) {
		printf("cannot find name\n");
		return NULL;
	}
	strcpy(user_auth->passwd_md5, value);

	return user_auth;
}

int user_add(char *name, char *passwd_md5)
{

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
