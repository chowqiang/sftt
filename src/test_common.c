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
	ctx->root = __strdup(buf);

	ret = mkdir(ctx->root, S_IRUSR | S_IWUSR);
	if (ret == -1) {
		perror("create test root directory failed");
		goto test_context_free;
	}

	return ctx;

test_context_free:
	if (ctx && ctx->root)
		mp_free(g_mp, ctx->root);

	if (ctx)
		mp_free(g_mp, ctx);

	return NULL;
}

char *test_context_get_root(struct test_context *ctx)
{
	return __strdup(ctx->root);
}

int test_context_add_dirs(struct test_context *ctx, const char *dirs[], int num)
{
	int i = 0, j = 0, ret = 0;

	if (ctx->dir_num + num > TEST_CONTEXT_MAX_DIRS_NUM)
		return -1;

	for (i = 0, j = ctx->dir_num; i < num; ++i, ++j) {
		ctx->dir_list[j] = path_join(ctx->root, dirs[i]);
		if (ctx->dir_list[j] == NULL) {
			return -1;
		}

		ret = mkdir(ctx->dir_list[j], S_IRUSR | S_IWUSR);
		if (ret == -1)
			return -1;
	}
	ctx->dir_num = j;

	return 0;
}

static int generate_one_cmd(struct test_context *ctx, struct test_cmd *cmd,
		char buf[], int len)
{
	int i = 0, ret = 0;

	ret = snprintf(buf, len, "%s ", cmd->cmd);
	for (i = 0; cmd->args[i]; ++i) {
		if (cmd->chroot_flags & BIT32(i)) {
			ret += snprintf(buf + ret, len - ret, "%s/%s ", ctx->root,
					cmd->args[i]);
		} else {
			ret += snprintf(buf + ret, len - ret, "%s ", cmd->args[i]);
		}
	}
	/* Remove the last space */
	buf[strlen(buf) - 1] = 0;

	return 0;
}

int test_context_generate_cmd_file(struct test_context *ctx, const char *fname,
		struct test_cmd *cmds, int num)
{
	int i = 0, ret = 0;
	FILE *fp = NULL;
	char buf[CMD_MAX_LEN];

	if (ctx->cmd_file)
		return -1;

	ctx->cmd_file = path_join(ctx->root, fname);
	fp = fopen(ctx->cmd_file, "w");
	if (fp == NULL)
		return -1;

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

	ctx->cmp_file = path_join(ctx->root, fname);
	fp = fopen(ctx->cmp_file, "w");
	if (fp == NULL)
		return -1;

	for (i = 0; list->files[i]; ++i) {
		if (list->chroot_flags & BIT32(i)) {
			path = path_join(ctx->root, list->files[i]);
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

	ctx->finish_file = path_join(ctx->root, finish_file);

	return 0;
}

static pid_t dummy_exec_test(struct test_context *ctx)
{
	pid_t pid;

	pid = fork();
	if (pid < 0) {
		perror("fork failed");
		return (pid_t)-1;
	} else if (pid == 0) {
		execl("/bin/touch", "touch", ctx->finish_file, NULL);
		perror("In exec(): ");
	}

	return pid;
}

int test_context_run_test(struct test_context *ctx)
{
	char buf[1024];
	pid_t pid;
	int status;

	printf("begin to test ...\n");

	printf("testing ...\n");

	pid = dummy_exec_test(ctx);
	printf("The pid of test process is: %d\n", pid);

	pid = wait(&status);
	printf("End of process: %d\n", pid);
	if (file_existed(ctx->finish_file)) {
		ctx->success = true;
	} else {
		ctx->success = false;
	}

	if (WIFEXITED(status)) {
		printf("The test process ended with exit(%d).\n", WEXITSTATUS(status));
	}
	if (WIFSIGNALED(status)) {
		printf("The test process ended with kill -%d.\n", WTERMSIG(status));
	}

	ctx->test_error = WEXITSTATUS(status);

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
