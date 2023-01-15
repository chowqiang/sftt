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
#include "utils.h"

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

void __test_get_path_entry_list(char *path)
{
	struct dlist *file_list;
	struct dlist_node *node;
	struct path_entry *entry;

	file_list = get_path_entry_list(path, NULL);
	if (file_list == NULL) {
		printf("cannot get path entry list: %s\n", path);
		return;
	}

	printf("path: %s\n", path);
	dlist_for_each(file_list, node) {
		entry = node->data;
		printf("%s, %s\n", entry->abs_path, entry->rel_path);
	}

}

void test_get_path_entry_list(void)
{
	char *path = "/home/zhoumin/tmp/1";
	char *path2 = "/home/zhoumin/tmp/2";
	char *path3 = "/tmp/sftt_test_fwd_get_2124247";

	__test_get_path_entry_list(path);
	__test_get_path_entry_list(path2);
	__test_get_path_entry_list(path3);
}

void test_dir_compare(void)
{
	char *path = "/home/zhoumin/tmp/1";
	char *path2 = "/home/zhoumin/tmp/2";

	if (dir_compare(path, path2) == 0) {
		printf("%s is same to %s\n", path, path2);
	} else {
		printf("%s is not same to %s\n", path, path2);
	}
}

void test_gen_random_files(void)
{
	struct file_gen_attr attrs[] = {
		{"a/e.txt", FILE_TYPE_FILE, 100000, DEFAULT_FILE_MODE},
		{"c/g.txt", FILE_TYPE_FILE, 200000, DEFAULT_FILE_MODE},
		{"a/d/h.txt", FILE_TYPE_FILE, 300000, DEFAULT_FILE_MODE},
		{"b/f/i/j.txt", FILE_TYPE_FILE, 400000, DEFAULT_FILE_MODE}
	};

	gen_files_by_template(attrs, ARRAY_SIZE(attrs), "/home/zhoumin/tmp/tmo");
}

void test_create_new_file_with_parent(void)
{
	char *fname = "/tmp/test/sfttd/a.txt";

	create_new_file_with_parent(fname, DEFAULT_FILE_MODE);
	if (!file_existed(fname)) {
		printf("create_new_file_with_parent not successed!\n");
	} else {
		printf("create_new_file_with_parent successed!\n");
	}
}

int main(void)
{
	//test_get_all_file_list();
	//test_mode();
	test_get_path_entry_list();
	//test_dir_compare();
	//test_gen_random_files();
	//test_create_new_file_with_parent();

	return 0;
}
