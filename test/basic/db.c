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

	printf("uid: %ld, name: %s\n", user_base->uid, user_base->name);

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
