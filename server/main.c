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

#include <unistd.h>
#include "debug.h"
#include "server.h"

extern int verbose_level;

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

int main(int argc, char **argv)
{
	char store_path[DIR_PATH_MAX_LEN];
	bool background = false;
	bool ret = false;
	char ch;

	if (argc < 2) {
		server_usage_help(-1);
	}

	memset(store_path, 0, sizeof(store_path));
	while ((ch = getopt(argc, argv, "s:d:v")) != -1) {
		switch (ch) {
		case 's':
			strncpy(store_path, optarg, sizeof(store_path) - 1);
			break;
		case 'd':
			background = true;
			break;
		case 'v':
			verbose_level++;
			break;
		case '?':
		default:
			printf("unknown args!\n");
			server_usage_help(-1);
		}
	}

	argc -= optind;
	if (argc != 1) {
		server_usage_help(-1);
	}

	set_server_debug_level(verbose_level);

	if (strcmp(argv[optind], "start") == 0) {
		sftt_server_start(store_path, background);
	} else if (strcmp(argv[optind], "restart") == 0) {
		sftt_server_restart(store_path, background);
	} else if (strcmp(argv[optind], "stop") == 0) {
		sftt_server_stop();
	} else if (strcmp(argv[optind], "status") == 0) {
		sftt_server_status();
	} else if (strcmp(argv[optind], "db") == 0) {
		sftt_server_db();
	} else {
		printf("unknown args!\n");
		server_usage_help(-1);
	}

	return 0;
}
