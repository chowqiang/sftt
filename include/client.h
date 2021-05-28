#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdlib.h>
#include "command.h"
#include "config.h"
#include "connect.h"
#include "dlist.h"
#include "mem_pool.h"
#include "option.h"
#include "session.h"
#include "user.h"

#define PROC_NAME			"sftt"

#define LOCAL_HOST			"127.0.0.1"
#define PORT_CACHE_FILE			"./.cache_port"
#define MAX_PORT_NUM			65536

#define IPV4_MAX_LEN			16	
#define CLIENT_MAX_CONNECT_NUM		4

#define SESSION_TYPE_FILE		1
#define SESSION_TYPE_DIR		2

#define EXEC_NAME_MAX_LEN		32
#define CMD_ARG_MAX_LEN			1024

#define CLIENT_LOG_DIR			"/var/log/"PROC_NAME"/"

struct path_entry {
	char abs_path[FILE_NAME_MAX_LEN];
	char rel_path[FILE_NAME_MAX_LEN];
};

struct path_entry_list {
	struct path_entry entry;
	struct path_entry_list *next;
};

struct file_input_stream {
	char target[FILE_NAME_MAX_LEN + 1];
	int cursor;
	FILE *fp;
	int (*get_next_buffer)(struct file_input_stream *fis, char *buffer, size_t size);
};

struct sftt_client {
	char ip[IPV4_MAX_LEN];
	int port;
	int configured_block_size;
	struct sock_connect connects[CLIENT_MAX_CONNECT_NUM];
	int connects_num;
	char session_file[FILE_NAME_MAX_LEN];
	int session_type;
	void *trans_session;
};

struct sftt_client_v2 {
	struct client_sock_conn conn_ctrl;
	struct dlist *conn_data;
	struct mem_pool *mp;
	struct user_base_info *uinfo;
	char session_id[SESSION_ID_LEN];
	char host[HOST_MAX_LEN];
    	struct sftt_client_config config;
};

struct thread_input_params {
	struct sock_connect connect;
	int index;
	int step;
	struct path_entry *pes;
	int pe_count;
};

struct file_trans_session{
        char ip[IPV4_MAX_LEN];
        int index;
        char md5;
	struct path_entry pe;
};

struct dir_trans_session{
        char ip[IPV4_MAX_LEN];
        struct file_trans_session *fts;
        int fts_count;
	struct path_entry pe;
};

struct path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix);

int find_unfinished_session(struct path_entry *pe, char *ip);

int file_trans_session_diff(struct file_trans_session *old_session, struct file_trans_session *new_seesion);

int dir_trans_session_diff(struct dir_trans_session *old_session, struct dir_trans_session *new_session);

int save_trans_session(struct sftt_client *client);

int file_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size);

int dir_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size);

int is_dir(char *file_name);

int is_file(char *file_name);

struct file_input_stream *create_file_input_stream(char *file_name);

void destory_file_input_stream(struct file_input_stream *fis);

int get_cache_port();

void set_cache_port(int port);

struct sftt_client *create_client(char *ip, struct sftt_client_config *config, int connects_num); 

void destory_sftt_client(struct sftt_client *client);

int sftt_client_ll_handler(void *obj, int argc, char *argv[], bool *argv_check);

void sftt_client_ll_usage(void);

void sftt_client_cd_usage(void);

int sftt_client_cd_handler(void *obj, int argc, char *argv[], bool *argv_check);

void sftt_client_help_usage(void);

int sftt_client_help_handler(void *obj, int argc, char *argv[], bool *argv_check);

void sftt_client_get_usage(void);

int sftt_client_get_handler(void *obj, int argc, char *argv[], bool *argv_check);

void sftt_client_put_usage(void);

int sftt_client_put_handler(void *obj, int argc, char *argv[], bool *argv_check);

int sftt_client_pwd_handler(void *obj, int argc, char *argv[], bool *argv_check);

void sftt_client_pwd_usage(void);

int sftt_client_his_handler(void *obj, int argc, char *argv[], bool *argv_check);

void sftt_client_his_usage(void);

void client_usage_help(int exitcode);

int init_sftt_client_v2(struct sftt_client_v2 *client, char *host, int port, char *user, char *passwd);

int show_options(char *host, char *user_name, char *password); 

bool parse_user_name(char *arg, char *user_name, int maxlen);

bool parse_host(char *arg, char *host, int maxlen);

bool parse_port(char *arg, int *port);

int reader_loop2(struct sftt_client_v2 *client);

static struct cmd_handler sftt_client_cmds[] = {
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
