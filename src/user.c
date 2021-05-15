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

struct user_base_info *find_user_base_by_name(char *name, int *pnum)
{
	int i, count;
	struct map *data;
	struct db_connect *db_con;
	char *err_msg, *value, *db_file;
	char sql[1024];
	struct user_base_info *tmp;

	*pnum = 0;
	db_file = get_user_db_file();
	db_con = create_db_connect(db_file);
	assert(db_con != NULL);

	sprintf(sql, "select * from user where name='%s'", name);
	count = db_select(db_con, sql, &data, &err_msg);
	if (count < 1)
		return NULL;

	printf("user count: %d\n", count);
	tmp = mp_malloc(g_mp, count * sizeof(struct user_base_info));
	assert(tmp != NULL);

	for (i = 0; i < count; ++i) {
		if (map_find(&data[i], str_equal, "uid", (void **)&value) == -1 || value == NULL) {
			printf("cannot find uid\n");
			return NULL;
		}
		tmp[i].uid = atoi(value);

		if (map_find(&data[i], str_equal, "name", (void **)&value) == -1 || value == NULL) {
			printf("cannot find name\n");
			return NULL;
		}
		strcpy(tmp[i].name, value);
	}

	*pnum = count;

	return tmp;
}

int user_add(char *name, char *passwd_md5)
{

}
