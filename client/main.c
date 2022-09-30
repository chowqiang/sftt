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
#include <unistd.h>
#include "autoconf.h"
#include "client.h"
#include "cmdline.h"
#include "debug.h"
#include "log.h"

extern int verbose_level;

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

int main(int argc, char **argv)
{
	char user_name[USER_NAME_MAX_LEN];
	char password[PASSWD_MD5_LEN];
	char host[HOST_MAX_LEN];
	char passwd_prompt[128];
	char builtin[32];
	char cmd_file[FILE_PATH_MAX_LEN];
	char *state_file = NULL;
	char ch;
	int port = -1;
	int ret;

	bool has_opt = false;
	enum run_mode mode = RUN_MODE_LOGIN;

	struct trans_info trans;
	struct sftt_client client;

	memset(user_name, 0, sizeof(user_name));
	memset(password, 0, sizeof(password));
	memset(host, 0, sizeof(host));
	memset(builtin, 0, sizeof(builtin));

	while ((ch = getopt(argc, argv, "b:f:u:p:P:s:h:v")) != -1) {
		switch (ch) {
		case 'b':
			strncpy(builtin, optarg, sizeof(builtin) - 1);
			mode = RUN_MODE_BUILTIN;
			break;
		case 'f':
			strncpy(cmd_file, optarg, sizeof(cmd_file) - 1);
			mode = RUN_MODE_CMD_FILE;
			break;
		case 'u':
			strncpy(user_name, optarg, sizeof(user_name) - 1);
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'P':
			strncpy(password, optarg, sizeof(password) - 1);
			break;
		case 's':
			state_file = __strdup(optarg);
			break;
		case 'h':
			strncpy(host, optarg, sizeof(host) - 1);
			break;
		case 'v':
			verbose_level++;
			break;
		case '?':
		default:
			printf("unknown args!\n");
			client_usage_help(-1);
		}
		has_opt = true;
	}

	if (has_opt) {
		argc -= optind;
		if (argc > 0) {
			client_usage_help(-1);
		}
	} else if (argc == 2) {
		ret = try_fetch_login_info(argv[1], user_name, host);
		if (ret == -1) {
			printf("parse login info failed!\n");
			client_usage_help(-1);
		}
	} else if (argc == 3) {
		ret = try_fetch_trans_info(argv[1], argv[2], user_name, host, &trans);
		if (ret == -1) {
			printf("parse trans info failed!\n");
			client_usage_help(-1);
		}
		printf("%s:%d, trans_type=%d|src=%s|dest=%s\n", __func__, __LINE__,
			trans.type, trans.src, trans.dest);
		mode = RUN_MODE_TRANS;
	}

	if (strlen(user_name) == 0) {
		printf("user name is invalid!\n");
		client_usage_help(-1);
	}

	if (strlen(host) == 0) {
		printf("host is invalid!\n");
		client_usage_help(-1);
	}

	if (mode == RUN_MODE_CMD_FILE) {
		if (!file_existed(cmd_file)) {
			printf("command file not existed: %s\n", cmd_file);
			return -1;
		}
	}

	if (mode == RUN_MODE_BUILTIN && strlen(builtin) == 0) {
		printf("builtin is invalid!\n");
		client_usage_help(-1);
	}

#ifdef CONFIG_CLIENT_DEFAULT_DEBUG_LEVEL
	set_client_debug_level(verbose_level, CONFIG_CLIENT_DEFAULT_DEBUG_LEVEL);
#elif defined(CONFIG_DEFAULT_DEBUG_LEVEL)
	set_client_debug_level(verbose_level, CONFIG_DEFAULT_DEBUG_LEVEL);
#else
	set_client_debug_level(verbose_level, "warn");
#endif

	if (strlen(password) == 0) {
		snprintf(passwd_prompt, 127, "%s@%s's password: ", user_name, host);
		if (get_pass(passwd_prompt, password, sizeof(password)) <= 0) {
			printf("password is invalid!\n");
			client_usage_help(-1);
		}
	}

#ifdef CONFIG_DEBUG
	show_options(host, user_name, password);
#endif

	if (init_sftt_client(&client, host, port, user_name, password,
				state_file) == -1) {
		printf("init sftt client failed!\n");
		exit(-1);
	}

	add_log(LOG_INFO, "client validate successfully!");

	if (mode == RUN_MODE_LOGIN) {
		reader_loop(&client);
	} else if (mode == RUN_MODE_BUILTIN) {
		return do_builtin(&client, builtin);
	} else if (mode == RUN_MODE_TRANS) {
		return do_trans(&client, &trans);
	} else if (mode == RUN_MODE_CMD_FILE) {
		return do_cmd_file(&client, cmd_file);
	} else {
		printf("unknown run mode!\n");
	}

	return 0;
}
