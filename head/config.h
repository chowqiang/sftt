#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string.h>

#define BUFFER_SIZE	10240

#define FILE_NAME_MAX_LEN	256

#define CONFIG_LINE_MAX_SIZE	1024
#define CONFIG_NAME_MAX_LEN 	256
#define CONFIG_VALUE_MAX_LEN	512

#define BLOCK_TYPE_FILE_NAME		"FILE:"
#define BLOCK_TYPE_DATA			"DATA:"
#define BLOCK_TYPE_FILE_END		"FEND:"
#define BLOCK_TYPE_SEND_COMPLETE 	"SCOM:"			

#define BLOCK_TYPE_SIZE			(sizeof(BLOCK_TYPE_DATA) - 1)

#define PACKET_LEN_SIZE			4

typedef struct sftt_server_config {
	char store_path[FILE_NAME_MAX_LEN + 1];
	int block_size;
} sftt_server_config;

typedef struct sftt_client_config {
	int block_size;
} sftt_client_config;

/**
 * get sftt server config
 *
 */
int get_sftt_server_config(sftt_server_config *ssc);

int get_sftt_client_config(sftt_client_config *scc);

#endif
