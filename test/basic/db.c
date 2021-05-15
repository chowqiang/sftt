#include <stdio.h>
#include "db.h"
#include "user.h"

void test(char *name)
{
	struct user_base_info *user;
	int num;

	user = find_user_base_by_name(name, &num);
	if (user == NULL)
		return ;
       	printf("user: %p, num: %d\n", user, num);	
	printf("uid: %d, name: %s\n", user->uid, user->name);
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
