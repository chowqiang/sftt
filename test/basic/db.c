#include <stdio.h>
#include "db.h"
#include "user.h"

void test_select(int argc, char *argv[])
{
	struct user_base_info *user_base;
	struct user_auth_info *user_auth;
	char *name;

	if (argc < 2) {
		printf("Usage: %s name\n", argv[0]);

		return ;
	}

	name = argv[1];
	user_base = find_user_base_by_name(name);
	if (user_base == NULL)
		return ;

	printf("uid: %d, name: %s\n", user_base->uid, user_base->name);

	user_auth = find_user_auth_by_name(name);
	if (user_auth == NULL)
		return ;

	printf("name: %s, passwd_md5: %s\n", user_auth->name, user_auth->passwd_md5);
}

void test_select_count(int argc, char *argv[])
{
	int count = get_user_count();

	printf("user count: %d\n", count);
}

int main(int argc, char *argv[])
{

	test_select_count(argc, argv);

	return 0;
}
