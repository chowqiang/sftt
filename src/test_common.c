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

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "bits.h"
#include "common.h"
#include "file.h"
#include "mem_pool.h"
#include "test_common.h"
#include "utils.h"

extern struct mem_pool *g_mp;

#define TEST_ROOT_DIR "/tmp/sftt_test_%s_%d"

static void sanity_check(void)
{
	assert(TYPE_INT_BITS >= 32);
}

struct test_context *test_context_create(const char *name)
{
	struct test_context *ctx;
	char buf[DIR_PATH_MAX_LEN];
	int ret;

	sanity_check();

	ctx = mp_malloc(g_mp, __func__, sizeof(struct test_context));
	if (ctx == NULL)
		return NULL;

	ctx->name = name;

	snprintf(buf, DIR_PATH_MAX_LEN, TEST_ROOT_DIR, name, (int)getpid());
	ctx->root_dir = __strdup(buf);

	ret = mkdir(ctx->root_dir, S_IRUSR | S_IWUSR);
	if (ret == -1) {
		perror("create test root directory failed");
		goto test_context_free;
	}

	return ctx;

test_context_free:
	if (ctx && ctx->root_dir)
		mp_free(g_mp, ctx->root_dir);

	if (ctx)
		mp_free(g_mp, ctx);

	return NULL;
}

char *test_context_get_root_dir(struct test_context *ctx)
{
	return __strdup(ctx->root_dir);
}

int test_context_add_dirs(struct test_context *ctx, const char *dirs[], int num)
{
	char *path;
	int i = 0, j = 0, ret = 0;

	if (ctx->dir_num + num > TEST_CONTEXT_MAX_DIRS_NUM)
		return -1;

	for (i = 0, j = ctx->dir_num; i < num; ++i, ++j) {
		ctx->dir_list[j] = __strdup(dirs[i]);
		path = path_join(ctx->root_dir, dirs[i]);
		if (path == NULL) {
			return -1;
		}

		ret = mkdir(path, S_IRUSR | S_IWUSR);
		if (ret == -1)
			return -1;
	}
	ctx->dir_num = j;

	return 0;
}

int test_context_gen_random_files(struct test_context *ctx, char *dir,
		struct file_gen_attr attrs[], int num)
{
	char *path = path_join(ctx->root_dir, dir);

	return gen_files_by_template(attrs, num, path);
}

int test_context_add_process(struct test_context *ctx, char *process_name,
		char *exec_file, int priority, char *state_file,
		bool (*is_started)(struct test_process *proc),
		const char *argv[], int argc)
{
	int i, j;
	struct test_process *proc;

	if (ctx == NULL || process_name == NULL || exec_file == NULL ||
			state_file == NULL)
		return -1;

	if (strlen(process_name) >= TEST_PROCESS_MAX_NAME_LEN)
		return -1;

	if (argc > (TEST_PROCESS_MAX_ARGS_NUM - 2))
		return -1;

	proc = mp_malloc(g_mp, __func__, sizeof(struct test_process));
	if (proc == NULL)
		return -1;

	strcpy(proc->name, process_name);
	proc->exec_file = __strdup(exec_file);
	proc->cmd_file = NULL;
	proc->state_file = path_join(ctx->root_dir, state_file);
	proc->is_started = is_started;
	proc->argv[0] = get_basename(exec_file);

	for (i = 1, j = 0; j < argc; ++i, ++j) {
		proc->argv[i] = __strdup(argv[j]);
	}
	proc->argv[i++] = "-s";
	proc->argv[i++] = proc->state_file;
	if (proc->cmd_file) {
		proc->argv[i++] = "-f";
		proc->argv[i++] = proc->cmd_file;
	}
	proc->argv[i] = NULL;

	PRIORITY_INIT_LIST_HEAD(&proc->list, priority);

	proc->pid = 0;
	proc->valid = false;

	priority_list_add(&proc->list, &ctx->proc_list);

	return 0;
}

static int generate_one_cmd(struct test_context *ctx, struct test_cmd *cmd,
		char buf[], int len)
{
	int i = 0, ret = 0;

	ret = snprintf(buf, len, "%s ", cmd->cmd);
	for (i = 0; cmd->args[i]; ++i) {
		if (cmd->chroot_flags & BIT32(i)) {
			ret += snprintf(buf + ret, len - ret, "%s/%s ", ctx->root_dir,
					cmd->args[i]);
		} else {
			ret += snprintf(buf + ret, len - ret, "%s ", cmd->args[i]);
		}
	}
	/* Remove the last space */
	buf[strlen(buf) - 1] = 0;

	return 0;
}

struct test_process *test_context_get_process(struct test_context *ctx, char *name)
{
	struct test_process *process = NULL;

	priority_list_for_each_entry(process, &ctx->proc_list, list)
		if (strcmp(process->name, name) == 0)
				return process;

	return NULL;
}

int test_context_generate_cmd_file(struct test_context *ctx, char *process_name,
		const char *fname, struct test_cmd *cmds, int num)
{
	int i = 0, ret = 0;
	FILE *fp = NULL;
	char buf[CMD_MAX_LEN];
	struct test_process *process = NULL;
	char *cmd_file = NULL;

	if (ctx == NULL || fname == NULL || cmds == NULL)
		return -1;

	if (process_name) {
		process = test_context_get_process(ctx, process_name);
		if (process == NULL)
			return -1;

		if (process->cmd_file)
			return -1;
	}

	cmd_file = path_join(ctx->root_dir, fname);
	fp = fopen(cmd_file, "w");
	if (fp == NULL)
		return -1;

	if (process) {
		process->cmd_file = cmd_file;
		process->valid = true;
	}

	for (i = 0; i < num; ++i) {
		generate_one_cmd(ctx, &cmds[i], buf, CMD_MAX_LEN);
		fwrite(buf, strlen(buf), 1, fp);
		write_new_line(fp);
	}

	fclose(fp);

	return 0;
}

int test_context_generate_cmp_file(struct test_context *ctx, const char *fname,
		struct test_cmp_file_list *list)
{
	char *path = NULL;
	FILE *fp = NULL;
	int i = 0;

	if (ctx->cmp_file)
		return -1;

	ctx->cmp_file = path_join(ctx->root_dir, fname);
	fp = fopen(ctx->cmp_file, "w");
	if (fp == NULL)
		return -1;

	for (i = 0; list->files[i]; ++i) {
		if (list->chroot_flags & BIT32(i)) {
			path = path_join(ctx->root_dir, list->files[i]);
		} else {
			path = list->files[i];
		}

		fwrite(path, strlen(path), 1, fp);
		write_new_line(fp);

		if (list->chroot_flags & BIT32(i)) {
			mp_free(g_mp, path);
		}
	}

	fclose(fp);

	return 0;
}

int test_context_add_finish_file(struct test_context *ctx, char *finish_file)
{
	if (ctx->finish_file)
		return -1;

	ctx->finish_file = path_join(ctx->root_dir, finish_file);

	return 0;
}

int start_one_test_process(struct test_context *ctx, struct test_process *proc)
{
	pid_t pid;

	pid = fork();
	if (pid < 0) {
		perror("fork failed");
		printf("run %s failed!\n", proc->name);
		return -1;
	} else if (pid == 0) {
		execv(proc->exec_file, proc->argv);
	}

	proc->pid = pid;

	if (!proc->is_started(proc)) {
		proc->valid = false;
		return -1;
	}

	proc->valid = true;
	return 0;
}

bool is_started_default(struct test_process *proc)
{
	bool timeout = false;
	time_t start = get_ts();

	while (!timeout) {
		if (file_existed(proc->state_file))
			return true;
		sleep(1);
		timeout = (get_ts() - start) > TEST_PROCESS_START_TIMEOUT;
	}

	return false;
}

int test_context_run_test(struct test_context *ctx)
{
	struct test_process *process;
	time_t start;
	bool timeout = false;

	printf("begin to test ...\n");

	printf("testing ...\n");

	priority_list_for_each_entry(process, &ctx->proc_list, list)
		if (start_one_test_process(ctx, process) == -1) {
			printf("start test process failed: %s\n", process->name);
			break;
		}

	/* wait test done */
	start = get_ts();
	while (!timeout) {
		if (file_existed(ctx->finish_file))
			break;
		sleep(1);
		timeout = (get_ts() - start) > TEST_COMPLETE_TIMEOUT;
	}

	if (!file_existed(ctx->finish_file)) {
		ctx->success = false;
	}

	priority_list_for_each_entry(process, &ctx->proc_list, list)
		if (process->need_kill)
			kill(process->pid, SIGTERM);

	printf("end test\n");

	return 0;
}

int test_context_get_result(struct test_context *ctx, bool *result,
		char *message, int len)
{
	*result = ctx->success;
	if (*result) {
		snprintf(message, len, "test ok!\n");
	} else {
		snprintf(message, len, "%s", strerror(ctx->test_error));
	}

	return 0;
}

int test_context_destroy(struct test_context *ctx)
{
	return 0;
}
