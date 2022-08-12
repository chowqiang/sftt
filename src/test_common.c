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

#include "mem_pool.h"
#include "test_common.h"

extern struct mem_pool *g_mp;

struct test_context *test_context_create(const char *name)
{
	return NULL;
}

int test_context_get_root(struct test_context *ctx, char *root, int len)
{
	return 0;
}

int test_context_add_dirs(struct test_context *ctx, char *dirs[], int num)
{
	return 0;
}

int test_context_add_misc_files(struct test_context *ctx, char *cmd_file,
		char *finish_file, char *cmp_file)
{
	return 0;
}

int test_context_run_test(struct test_context *ctx)
{
	return 0;
}

int test_context_get_result(struct test_context *ctx, bool *result,
		char *message, int len)
{
	return 0;
}
