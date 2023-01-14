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
#include <sys/types.h>
#include "file.h"
#include "priority_list.h"

#define TEST_CONTEXT_MAX_DIRS_NUM 	16

#define TEST_CMD_MAX_ARGS_NUM	 	TYPE_INT_BITS
#define TEST_CMP_MAX_FILE_NUM		TYPE_INT_BITS

#define TEST_PROCESS_MAX_NAME_LEN	32
#define TEST_PROCESS_MAX_ARGS_NUM	32

#define TEST_PROCESS_PRIORITY_SERVER	1
#define TEST_PROCESS_PRIORITY_CLIENT_1	2
#define TEST_PROCESS_PRIORITY_CLIENT_2	3

#define TEST_PROCESS_START_TIMEOUT	10

#define TEST_COMPLETE_TIMEOUT		600

struct test_process {
	char name[TEST_PROCESS_MAX_NAME_LEN];
	char *exec_file;
	char *cmd_file;
	char *state_file;
	char *argv[TEST_PROCESS_MAX_ARGS_NUM + 2];
	struct priority_list_head list;
	pid_t pid;
	bool valid;
	bool (*is_started)(struct test_process *proc);
	bool need_kill;
};

struct test_context {
	const char *name;
	char *root_dir;
	char *dir_list[TEST_CONTEXT_MAX_DIRS_NUM];
	int dir_num;
	struct test_process server;
	struct priority_list_head proc_list;
	char *cmp_file;
	char *finish_file;
	bool success;
	int test_error;
};

struct test_cmd {
	const char *cmd;
	char *args[TEST_CMD_MAX_ARGS_NUM];
	int chroot_flags;
};

struct test_cmp_file_list {
	char *files[TEST_CMP_MAX_FILE_NUM];
	int chroot_flags;
};

struct test_context *test_context_create(const char *name);

char *test_context_get_root_dir(struct test_context *ctx);

int test_context_add_dirs(struct test_context *ctx, const char *dirs[], int num);

int test_context_add_process(struct test_context *ctx, char *process_name,
		char *exec_file, int priority, char *state_file,
		bool (*is_started)(struct test_process *proc),
		bool need_kill, const char *argv[], int argc);

int test_context_generate_cmd_file(struct test_context *ctx, char *process_name,
		const char *fname, struct test_cmd *cmds, int num);

int test_context_generate_cmp_file(struct test_context *ctx, const char *fname,
		struct test_cmp_file_list *list);

int test_context_add_finish_file(struct test_context *ctx, char *finish_file);

int test_context_gen_random_files(struct test_context *ctx, char *dir,
		struct file_gen_attr attrs[], int num);

int test_context_run_test(struct test_context *ctx);

int test_context_get_result(struct test_context *ctx, bool *result,
		char *message, int len);

int test_context_destroy(struct test_context *ctx);

bool is_started_default(struct test_process *proc);

#endif
