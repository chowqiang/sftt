#include <stdio.h>
#include "file.h"
#include "user.h"

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
	int i;

	for (i = 0; users[i].uid != -1; ++i) {
		if (!strcmp(name, users[i].name)) {
			return &users[i];
		}
	}

	return NULL;
}

int user_add(char *name, char *passwd_md5)
{

}
