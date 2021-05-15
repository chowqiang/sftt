#include <stdio.h>
#include "db.h"
#include "user.h"

void test(char *name)
{
	struct user_base_info *user_base;
	struct user_auth_info *user_auth;

	user_base = find_user_base_by_name(name);
	if (user_base == NULL)
		return ;
	printf("uid: %d, name: %s\n", user_base->uid, user_base->name);

	user_auth = find_user_auth_by_name(name);
	if (user_auth == NULL)
		return ;
	printf("name: %s, passwd_md5: %s\n", user_auth->name, user_auth->passwd_md5);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s name\n", argv[0]);

		return -1;
	}

	test(argv[1]);

	return 0;
}
