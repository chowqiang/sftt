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

#include "server.h"

extern struct sftt_option sftt_server_opts[];

int main(int argc, char **argv) {
	int optind = 1;
	char *optarg = NULL;
	const struct sftt_option *opt = NULL;
	bool ret = false;
	char store_path[DIR_PATH_MAX_LEN];
	enum option_index opt_idx = UNKNOWN_OPT;
	bool background = false;

	if (argc < 2) {
		server_usage_help(-1);
	}

	memset(store_path, 0, sizeof(store_path));
	for (;;) {
		if (optind >= argc) {
			break;
		}
		opt = lookup_opt(argc, argv, &optarg, &optind, sftt_server_opts);
		if (opt == NULL) {
			printf("invalid option: %s\n", argv[optind]);
			server_usage_help(-1);
		}
		switch (opt->index) {
		case START:
			opt_idx = START;
			break;
		case RESTART:
			opt_idx = RESTART;
			break;
		case STOP:
			opt_idx = STOP;
			break;
		case STATUS:
			opt_idx = STATUS;
			break;
		case DB:
			opt_idx = DB;
			break;
		case DAEMON:
			background = true;
			break;
		case STORE:
			ret = parse_store_path(optarg, store_path, DIR_PATH_MAX_LEN - 1);
			++optind;
			break;
		default:
			printf("unknown parameter: %s\n", argv[optind]);
			server_usage_help(-1);
			break;
		}
	}

	if (optind < argc) {
		printf("unknown parameters: %s\n", argv[optind]);
		server_usage_help(-1);
	}

	switch (opt_idx) {
	case START:
		sftt_server_start(store_path, background);
		break;
	case RESTART:
		sftt_server_restart(store_path, background);
		break;
	case STOP:
		sftt_server_stop();
		break;
	case STATUS:
		sftt_server_status();
		break;
	case DB:
		sftt_server_db();
		break;
	}

	return 0;
}
