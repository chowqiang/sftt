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
#include "bits.h"
#include "common.h"
#include "test_common.h"
#include "utils.h"

#define TEST_NAME	"get"

#define CLIENT_DIR	"client"
#define SERVER_DIR	"server"

#define TEST_STATE_FILE_SERVER	"server.st"
#define TEST_STATE_FILE_CLIENT	"client.st"
#define TEST_CMD_FILE		"cmd_file"
#define TEST_CMP_FILE		"cmp_file"
#define TEST_FINISH_FILE	"done"

#define CLIENT_PROCESS	"client"
#define SERVER_PROCESS	"server"

#ifndef CLIENT_PATH
#error "please define CLIENT_PATH as client path"
#endif

#ifndef SERVER_PATH
#error "please define SERVER_PATH as server path"
#endif

static const char *dirs[] = {
	SERVER_DIR,
	CLIENT_DIR,
};

struct file_gen_attr attrs[] = {
	{"a/e.txt", FILE_TYPE_FILE, 100000, 0666},
	{"c/g.txt", FILE_TYPE_FILE, 200000, 0666},
	{"a/d/h.txt", FILE_TYPE_FILE, 300000, 0666},
	{"b/f/i/j.txt", FILE_TYPE_FILE, 400000, 0666}
};

static struct test_cmd cmds[] = {
	{
		.cmd = "w",
		.args = {NULL},
		.chroot_flags = 0
	},
	{
		.cmd = "get",
		.args = {SERVER_DIR, CLIENT_DIR, NULL},
		.chroot_flags = BIT32(0) | BIT32(1)
	},
	{
		.cmd = "touch",
		.args = {TEST_FINISH_FILE, NULL},
		.chroot_flags = BIT32(0)
	}
};

static struct test_cmp_file_list cmp_file_list = {
	.files = {CLIENT_DIR, SERVER_DIR, NULL},
	.chroot_flags = BIT32(0) | BIT32(1)
};

static const char *client_args[] = {
	"-h",
	"127.0.0.1",
	"-u",
	"root",
	"-p",
	"root"
};

static const char *server_args[] = {
	"-d"
};

/*
 * 1. Generate test dir in server
 * 2. Generate random file and dir in server test dir
 * 3. Generate test dir in client
 * 4. Start server
 * 5. Start client and read commands from file
 * 6. Client run commands
 * 7. Check file md5 between server random file and client gotten file
 * 8. Check dir between server random dir and client gotten dir
 * 9. Give checked results
 */
int test_get(int argc, char *argv[])
{
	struct test_context *ctx = NULL;
	char *root = NULL;
	char res_msg[1024];
	bool test_result;

	ctx = test_context_create(TEST_NAME);
	if (ctx == NULL)
		return -1;

	test_context_add_dirs(ctx, dirs, ARRAY_SIZE(dirs));

	test_context_gen_random_files(ctx, SERVER_DIR, attrs, ARRAY_SIZE(attrs));

	test_context_add_process(ctx, SERVER_PROCESS, SERVER_PATH,
			TEST_PROCESS_PRIORITY_SERVER, TEST_STATE_FILE_SERVER,
			is_started_default, true, server_args,
			ARRAY_SIZE(server_args));

	test_context_add_process(ctx, CLIENT_PROCESS, CLIENT_PATH,
			TEST_PROCESS_PRIORITY_CLIENT_1, TEST_STATE_FILE_CLIENT,
			is_started_default, true, client_args,
			ARRAY_SIZE(client_args));

	test_context_generate_cmd_file(ctx, CLIENT_PROCESS, TEST_CMD_FILE,
			cmds, ARRAY_SIZE(cmds));

	test_context_generate_cmp_file(ctx, TEST_CMP_FILE, &cmp_file_list);

	test_context_add_finish_file(ctx, TEST_FINISH_FILE);

	test_context_run_test(ctx);

	test_context_get_result(ctx, &test_result, res_msg, sizeof(res_msg));
	if (test_result) {
		printf("test successfully!\n");
	} else {
		printf("test failed: %s\n", res_msg);
	}

	test_context_destroy(ctx);

	return 0;
}

int main(int argc, char *argv[])
{
	test_get(argc, argv);

	return 0;
}
