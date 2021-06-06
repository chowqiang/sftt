#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string.h>
#include "req_rsp.h"

#define BUFFER_SIZE	10240

#define CONFIG_LINE_MAX_SIZE	1024
#define CONFIG_NAME_MAX_LEN 	256
#define CONFIG_VALUE_MAX_LEN	512

/**
 * host info
 */
#define HOST_MAX_LEN		32


struct sftt_server_config {
	char store_path[FILE_NAME_MAX_LEN];
	char log_dir[DIR_PATH_MAX_LEN];
	int block_size;
	int update_th;
};

struct sftt_client_config {
	int block_size;
	char log_dir[DIR_PATH_MAX_LEN];
};

/**
 * get sftt server config
 */
int get_sftt_server_config(struct sftt_server_config *ssc);

int get_sftt_client_config(struct sftt_client_config *scc);

#endif
