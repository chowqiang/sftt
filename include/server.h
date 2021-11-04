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

#ifndef __HEAD_SERVER__
#define __HEAD_SERVER__
#include <stdio.h>
#include "config.h"
#include "net_trans.h"
#include "option.h"
#include "db.h"
#include "version.h"

#define ASSERT_STORE_PATH_LEN(opt, path, len)	\
do {	\
	if (!(strlen(path) < len)) {	\
				printf("%s: work directory \"%s\" too long! That should be less than %d.\n", opt, path, len);	\
				exit(-1);	\
			}	\
} while (false)

#define PROC_NAME	"sfttd"

#define SFTT_SERVER_SHMKEY_FILE	"/var/lib/"PROC_NAME"/shmkey"

#define SFTT_SERVER_SHM_SIZE	4096

enum sftt_server_status {
	READY,
	SERVERING,
	STOPED
};

/*
 */
struct sftt_server {
	int main_sock;
	int main_port;
	pthread_t log_tid;
	uint64_t last_update_ts;
	enum sftt_server_status status;
	struct version_info ver;
	struct sftt_server_config conf;
	struct client_session sessions[MAX_CLIENT_NUM];
	struct pthread_mutex *pm;
};

struct sftt_server_stat {
	pid_t main_pid;
	int main_sock;
	int main_port;
	uint64_t last_update_ts;
	enum sftt_server_status status;
	struct sftt_server_config conf;
};

void server_init_func();
int  server_consult_block_size(int connect_fd, unsigned char *buff, int server_block_size);
void server_file_resv(int connect_fd, int consulted_block_size, struct sftt_server_config init_conf);
FILE *server_creat_file(struct sftt_packet *sp, struct sftt_server_config init_conf, char * data_buff);
void server_transport_data_to_file(FILE * fd, struct sftt_packet * sp);
void is_exit(char * filepath);

int sftt_server_start(char *store_path, bool background);
int sftt_server_restart(char *store_path, bool background);
int sftt_server_stop(void);

bool parse_store_path(char *optarg, char *store_path, int max_len);

void sftt_server_status(void);
void sftt_server_db(void);
void sftt_server_exit(int sig);
void server_usage_help(int exitcode);
#endif
