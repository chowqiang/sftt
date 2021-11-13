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

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dlist.h"
#include "file.h"

void test_path_ops(void)
{
	char *path = "../../var/sftt/root";
	char *rp = realpath(path, NULL);
	char pwd[128];
	
	getcwd(pwd, 127);

	printf("%s\n", pwd);
	
	if (rp == NULL) {
		printf("get realpath failed!\n");
	} else {
		printf("%s\n", realpath(rp, NULL));
		printf("%s\n", basename(rp));	
	}
}

void test_get_all_file_list(void)
{
	struct dlist *list;
	struct dlist_node *node;

	list = get_all_file_list("/etc/acpi/");
#if 1
	dlist_for_each(list, node)
		printf("%s\n", (char *)node->data);
#endif
}

void test_mode(void)
{
	char *path = "/root/sftt/README.md";
	char *path2 = "/root/sftt/README.md2";
	FILE *fp = NULL;

	mode_t mode = file_mode(path);
	printf("0x%0x\n", mode);

	fp = fopen(path2, "w");
	fclose(fp);

	set_file_mode(path2, mode);

	mode = file_mode(path);
	printf("0x%0x\n", mode);
}

int main(void)
{
	test_get_all_file_list();
	//test_mode();

	return 0;
}
