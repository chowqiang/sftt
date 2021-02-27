#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdlib.h>
#include "command.h"
#include "config.h"
#include "connect.h"
#include "dlist.h"
#include "memory_pool.h"
#include "option.h"
#include "session.h"
#include "user.h"

#define LOCAL_HOST				"127.0.0.1"
#define PORT_CACHE_FILE			"./.cache_port"
#define MAX_PORT_NUM			65536

#define IPV4_MAX_LEN			16	
#define CLIENT_MAX_CONNECT_NUM	4

#define SESSION_TYPE_FILE		1
#define SESSION_TYPE_DIR		2

#define EXEC_NAME_MAX_LEN		32
#define CMD_ARG_MAX_LEN			1024

sftt_option sftt_client_opts[] = {
	{"-u", USER, HAS_ARG},
	{"-h", HOST, HAS_ARG},
	{"-P", PORT, HAS_ARG},
	{"-p", PASSWORD, NO_ARG},
	{NULL, -1, NO_ARG}
};

struct user_cmd {
	const char *name;
	int argc;
	char **argv;
};

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
	char ip[IPV4_MAX_LEN];
	int port;
	int configured_block_size;
	sock_connect connects[CLIENT_MAX_CONNECT_NUM];
	int connects_num;
	char session_file[FILE_NAME_MAX_LEN];
	int session_type;
	void *trans_session;
} sftt_client;

typedef struct {
	client_sock_conn conn_ctrl;
	dlist *conn_data;
	memory_pool *mp;
	user_info *uinfo;
	client_session *session;
	char host[HOST_MAX_LEN];
} sftt_client_v2;

typedef struct {
	sock_connect connect;
	int index;
	int step;
	path_entry *pes;
	int pe_count;
} thread_input_params;

typedef struct {
        char ip[IPV4_MAX_LEN];
        int index;
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

int find_unfinished_session(path_entry *pe, char *ip);

int file_trans_session_diff(file_trans_session *old_session, file_trans_session *new_seesion);

int dir_trans_session_diff(dir_trans_session *old_session, dir_trans_session *new_session);

int save_trans_session(sftt_client *client);

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

int sftt_client_ll_handler(int argc, char *argv[]);

void sftt_client_ll_usage(void);

void sftt_client_cd_usage(void);

int sftt_client_cd_handler(int argc, char *argv[]);

void sftt_client_help_usage(void);

int sftt_client_help_handler(int argc, char *argv[]);

void sftt_client_get_usage(void);

int sftt_client_get_handler(int argc, char *argv[]);

void sftt_client_put_usage(void);

int sftt_client_put_handler(int argc, char *argv[]);

int sftt_client_pwd_handler(int argc, char *argv[]);

void sftt_client_pwd_usage(void);

int sftt_client_his_handler(int argc, char *argv[]);

void sftt_client_his_usage(void);

static struct command sftt_client_cmds[] = {
	{
		.name = "help",
		.fn = sftt_client_help_handler,
		.help = "show sftt client help info",
		.usage = sftt_client_help_usage,
	},
	{
		.name = "ll",
		.fn = sftt_client_ll_handler,
		.help = "list directory contents using a long listing format",
		.usage = sftt_client_ll_usage,
	},
	{
		.name = "cd",
		.fn = sftt_client_cd_handler,
		.help = "change work directory",
		.usage = sftt_client_cd_usage,
	},
	{
		.name = "pwd",
		.fn = sftt_client_pwd_handler,
		.help = "get current directory",
		.usage = sftt_client_pwd_usage,
	},
	{
		.name = "get",
		.fn = sftt_client_get_handler,
		.help = "get the file on server",
		.usage = sftt_client_get_usage,
	},
	{
		.name = "put",
		.fn = sftt_client_put_handler,
		.help = "put the file to server",
		.usage = sftt_client_put_usage,
	},
	{
		.name = "his",
		.fn = sftt_client_his_handler,
		.help = "get history command",
		.usage = sftt_client_his_usage,
	},
	{
		.name = NULL,
		.fn = NULL,
		.help = "",
		.usage = NULL,
	},
};
#endif
