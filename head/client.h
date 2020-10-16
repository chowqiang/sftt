#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "config.h"

#define LOCAL_HOST				"127.0.0.1"
#define PORT_CACHE_FILE			"./.cache_port"
#define MAX_PORT_NUM			65536

#define IPV4_MAX_LEN			16	
#define CLIENT_MAX_CONNECT_NUM		4	

typedef struct path_entry {
	char abs_path[FILE_NAME_MAX_LEN];
	char rel_path[FILE_NAME_MAX_LEN];
} path_entry;

typedef struct path_entry_list {
	path_entry entry;
	struct path_entry_list *next;
} path_entry_list;

typedef struct file_input_stream {
	char target[FILE_NAME_MAX_LEN + 1];
	int cursor;
	FILE *fp;
	int (*get_next_buffer)(struct file_input_stream *fis, char *buffer, size_t size);
} file_input_stream;

typedef struct {
	int sock;
	int block_size;
} sock_connect;

typedef struct {
	char ip[IPV4_MAX_LEN];
	int port;	
	int configured_block_size;
	sock_connect connects[CLIENT_MAX_CONNECT_NUM];
	int connects_num;
} sftt_client;

typedef struct {
	sock_connect connect;
	int index;
	int step;
	path_entry *pes;
	int pe_count;
} thread_input_params;

typedef struct {
        char ip[IPV4_MAX_LEN];
        int block_size;
        int block_index;
        char md5;
	path_entry pe;
} file_trans_session;

typedef struct {
        char ip[IPV4_MAX_LEN];
        file_trans_session *fts;
        int fts_count;
	path_entry pe;
} dir_trans_session;

path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix);

void unknow_session();

int start_file_trans_session();

int start_dir_trans_session();

int file_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size);

int dir_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size);

int is_dir(char *file_name);

int is_file(char *file_name);

file_input_stream *create_file_input_stream(char *file_name);

void destory_file_input_stream(file_input_stream *fis);

int get_cache_port();

void set_cache_port(int port);

sftt_client *create_client(char *ip, sftt_client_config *config, int connects_num); 

void destory_sftt_client(sftt_client *client);

#endif
