#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "config.h"

#define LOCAL_HOST				"127.0.0.1"
#define PORT_CACHE_FILE			"./.cache_port"
#define MAX_PORT_NUM			65536

#define IPV4_MAX_LEN			16	
#define CLIENT_SOCKETS_MAX_NUM	16

typedef struct file_input_stream {
	char target[FILE_NAME_MAX_LEN + 1];
	int cursor;
	FILE *fp;
	int (*get_next_buffer)(struct file_input_stream *fis, char *buffer, size_t size);
} file_input_stream;

typedef struct {
	char ip[IPV4_MAX_LEN];
	int port;	
	int socks[CLIENT_SOCKETS_MAX_NUM];
	int socks_num;
} sftt_client;

int file_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size);

int dir_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size);

int is_dir(char *file_name);

int is_file(char *file_name);

file_input_stream *create_file_input_stream(char *file_name);

void destory_file_input_stream(file_input_stream *fis);

int get_cache_port();

void set_cache_port(int port);

sftt_client *create_client(char *ip);

void destory_sftt_client(sftt_client *client);

#endif
