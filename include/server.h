#ifndef __HEAD_SERVER__
#define __HEAD_SERVER__
#include <stdio.h>
#include "config.h"
#include "net_trans.h"
#include "option.h"
#include "db.h"
//#include "encrypt.h"

#define ASSERT_STORE_PATH_LEN(opt, path, len)	\
do {	\
	if (!(strlen(path) < len)) {	\
				printf("%s: work directory \"%s\" too long! That should be less than %d.\n", opt, path, len);	\
				exit(-1);	\
			}	\
} while (false)

#define SFTT_SERVER_SHMKEY_FILE	"/var/lib/"PROC_NAME"/shmkey"

#define SFTT_SERVER_SHM_SIZE	4096

#define MAX_CHILD_NUM			128

struct sftt_option sftt_server_opts[] = {
	{"start", START, OPT_ARG},
	{"restart", RESTART, OPT_ARG},
	{"stop", STOP, NO_ARG},
	{"status", STATUS, NO_ARG},
	{"db", DB, NO_ARG},
	{NULL, -1, NO_ARG}
};

enum sftt_server_status {
	READY,
	RUNNING,
	NOT_RUNNING,
};

enum process_status {
	ACTIVE,
	EXITED
};

enum client_status {
	VALIDATED,
	INVALIDATED
};

struct child_info {
	pid_t pid;
	enum process_status status;
};

/*
 *
 */
struct client_info {
	int status;
	char session_id[SESSION_ID_LEN + 1];
	char pwd[DIR_PATH_MAX_LEN];
};

struct client_info *client_info_construct(void);

void client_info_deconstruct(struct client_info *ptr);

/*
 */
struct sftt_server {
	int main_sock;
	int main_port;
	uint64_t last_update_ts;
	enum sftt_server_status status;
	struct sftt_server_config conf;
};

struct sftt_server_info {
	pid_t main_pid;
	pid_t log_pid;
	struct sftt_server server;
	struct child_info child_list[MAX_CHILD_NUM];
};

void server_init_func();
int  server_consult_block_size(int connect_fd,char *buff, int server_block_size);
void server_file_resv(int connect_fd, int consulted_block_size, struct sftt_server_config init_conf);
FILE *server_creat_file(struct sftt_packet *sp, struct sftt_server_config init_conf, char * data_buff);
void server_transport_data_to_file(FILE * fd, struct sftt_packet * sp);
void is_exit(char * filepath);






#endif
