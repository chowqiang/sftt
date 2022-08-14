#ifndef _TEST_COMMON_H_
#define _TEST_COMMON_H_

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

#include <stdbool.h>

#define TEST_CONTEXT_MAX_DIRS_NUM 	16
#define MAX_ARGS_NUM			sizeof(int)
#define MAX_CMP_FILE_NUM		TYPE_INT_BITS

struct test_process {

};

struct test_context {
	const char *name;
	char *root;
	char *dir_list[TEST_CONTEXT_MAX_DIRS_NUM];
	int dir_num;
	char *cmd_file;
	char *cmp_file;
	char *finish_file;
	bool success;
	int test_error;
};

struct test_cmd {
	const char *cmd;
	char *args[MAX_ARGS_NUM];
	int chroot_flags;
};

struct test_cmp_file_list {
	char *files[MAX_CMP_FILE_NUM];
	int chroot_flags;
};

struct test_context *test_context_create(const char *name);

char *test_context_get_root(struct test_context *ctx);

int test_context_add_dirs(struct test_context *ctx, const char *dirs[], int num);

int test_context_generate_cmd_file(struct test_context *ctx, const char *fname,
		struct test_cmd *cmds, int num);

int test_context_generate_cmp_file(struct test_context *ctx, const char *fname,
		struct test_cmp_file_list *list);

int test_context_add_finish_file(struct test_context *ctx, char *finish_file);

int test_context_run_test(struct test_context *ctx);

int test_context_get_result(struct test_context *ctx, bool *result,
		char *message, int len);

int test_context_destroy(struct test_context *ctx);

#endif
