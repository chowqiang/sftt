#ifndef __HEAD_SERVER__
#define __HEAD_SERVER__
#include <stdio.h>
#include "config.h"
#include "net_trans.h"
#include "option.h"
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

sftt_option sftt_server_opts[] = {
	{"start", START, OPT_ARG},
	{"restart", RESTART, OPT_ARG},
	{"stop", STOP, NO_ARG},
	{NULL, -1, NO_ARG}
};

enum sftt_server_status {
	RUNNING,
	NOT_RUNNING,
};

typedef struct {
	pid_t pid;
	enum sftt_server_status status;
	char store_path[DIR_PATH_MAX_LEN];
} sftt_server_info;

typedef struct {
	int main_sock;
	uint64_t last_update_ts;
	sftt_server_config conf;
} sftt_server;

void server_init_func();
int  server_consult_block_size(int connect_fd,char *buff,int server_block_size);
void server_file_resv(int connect_fd , int consulted_block_size, sftt_server_config init_conf);
FILE *server_creat_file(sftt_packet *sp ,sftt_server_config  init_conf, char * data_buff);
void server_transport_data_to_file(FILE * fd,sftt_packet * sp);
void is_exit(char * filepath);






#endif
