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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdlib.h>
#include "command.h"
#include "config.h"
#include "connect.h"
#include "dlist.h"
#include "file.h"
#include "friend.h"
#include "mem_pool.h"
#include "option.h"
#include "packet.h"
#include "session.h"
#include "thread.h"
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

#define PROMPT_MAX_LEN			1024

enum trans_type {
	TRANS_GET,
	TRANS_PUT,
};

enum run_mode {
	RUN_MODE_LOGIN,
	RUN_MODE_TRANS,
	RUN_MODE_BUILTIN,
	RUN_MODE_UNKNOWN,
};

enum task_status {
	TASK_STATUS_RUNNING,
	TASK_STATUS_COMPLETE,
	TASK_STATUS_ABORT,
};

struct trans_info {
	enum trans_type type;
	char src[FILE_NAME_MAX_LEN];
	char dest[FILE_NAME_MAX_LEN];
};

struct file_input_stream {
	char target[FILE_NAME_MAX_LEN + 1];
	int cursor;
	FILE *fp;
	int (*get_next_buffer)(struct file_input_stream *fis, char *buffer,
			size_t size);
};

struct sftt_client {
	char ip[IPV4_MAX_LEN];
	int port;
	int configured_block_size;
	int connects_num;
	char session_file[FILE_NAME_MAX_LEN];
	int session_type;
	void *trans_session;
};

struct peer_task {
	pthread_t tid;
	char session_id[SESSION_ID_LEN];
	int sock;
	enum task_status status;
	struct list_head list;
};

struct peer_task_handler {
	pthread_t tid;
	int port;
	int sock;
	struct pthread_mutex *pm;
	struct peer_task *tasks;
};

struct sftt_client_v2 {
	struct client_sock_conn main_conn;
	struct list_head task_conns;
	struct pthread_mutex *tcs_lock;
	struct thread_info conn_mgr;
	struct mem_pool *mp;
	struct version_info ver;
	struct user_base_info uinfo;
	char session_id[SESSION_ID_LEN];
	char host[HOST_MAX_LEN];
	char pwd[DIR_PATH_MAX_LEN];
    	struct sftt_client_config config;
	struct list_head friends;
};

struct thread_input_params {
	int index;
	int step;
	struct path_entry *pes;
	int pe_count;
};

struct file_trans_session {
        char ip[IPV4_MAX_LEN];
        int index;
        char md5;
	struct path_entry pe;
};

struct dir_trans_session {
        char ip[IPV4_MAX_LEN];
        struct file_trans_session *fts;
        int fts_count;
	struct path_entry pe;
};

/*
 * Builtin test scripts struct.
 */
struct builtin_script {
        const char *name;	/* very short name for -b ... */
        const char *desc;	/* short description */
        const char *script;	/* actual pgbench script */
};

/*
 * Todo: need delete?
 */
int find_unfinished_session(struct path_entry *pe, char *ip);
int file_trans_session_diff(struct file_trans_session *old_session,
		struct file_trans_session *new_seesion);
int dir_trans_session_diff(struct dir_trans_session *old_session,
		struct dir_trans_session *new_session);
int save_trans_session(struct sftt_client *client);

/*
 * Todo: need delete?
 */
int file_get_next_buffer(struct file_input_stream *fis, char *buffer,
		size_t size);
int dir_get_next_buffer(struct file_input_stream *fis, char *buffer,
		size_t size);

/*
 * Todo: need delete?
 */
struct file_input_stream *create_file_input_stream(char *file_name);
void destroy_file_input_stream(struct file_input_stream *fis);

/*
 * Todo: need delete?
 */
int get_cache_port(void);
void set_cache_port(int port);

/*
 * Create and destroy sftt client.
 */
struct sftt_client *create_client(char *ip, struct sftt_client_config *config,
		int connects_num);
void destroy_sftt_client(struct sftt_client *client);

/*
 * List directory contents in long format.
 */
int sftt_client_ll_handler(void *obj, int argc, char *argv[], bool *argv_check);
void sftt_client_ll_usage(void);

/*
 * Change directory.
 */
void sftt_client_cd_usage(void);
int sftt_client_cd_handler(void *obj, int argc, char *argv[], bool *argv_check);

/*
 * Show help info.
 */
void sftt_client_help_usage(void);
int sftt_client_help_handler(void *obj, int argc, char *argv[], bool *argv_check);

/*
 * Get files from server to client.
 */
void sftt_client_get_usage(void);
int sftt_client_get_handler(void *obj, int argc, char *argv[], bool *argv_check);

/*
 * Put file from client to server.
 */
void sftt_client_put_usage(void);
int sftt_client_put_handler(void *obj, int argc, char *argv[], bool *argv_check);

/*
 * Return working directory name.
 */
int sftt_client_pwd_handler(void *obj, int argc, char *argv[], bool *argv_check);
void sftt_client_pwd_usage(void);

/*
 * Show history command.
 */
int sftt_client_his_handler(void *obj, int argc, char *argv[], bool *argv_check);
void sftt_client_his_usage(void);

/*
 * Show memory pool stat info.
 */
int sftt_client_mps_handler(void *obj, int argc, char *argv[], bool *argv_check);
void sftt_client_mps_usage(void);

void client_usage_help(int exitcode);
int init_sftt_client_v2(struct sftt_client_v2 *client, char *host, int port,
		char *user, char *passwd);
int show_options(char *host, char *user_name, char *password); 

/*
 * Parse user's input.
 */
bool parse_user_name(char *arg, char *user_name, int maxlen);
bool parse_host(char *arg, char *host, int maxlen);
bool parse_port(char *arg, int *port);

int reader_loop2(struct sftt_client_v2 *client);

int try_fetch_login_info(char *input, char *user_name, char *host);
int try_fetch_trans_info(char *arg1, char *arg2, char *user_name, char *host,
		struct trans_info *trans);
int do_trans(struct sftt_client_v2 *client, struct trans_info *trans);
int do_builtin(struct sftt_client_v2 *client, char *builtin);

int recv_one_file_by_get_resp(struct sftt_client_v2 *client,
		struct sftt_packet *resp_packet, struct common_resp *com_resp,
		char *target, bool *has_more);

/*
 * Enter or exit direct command mode.
 */
int sftt_client_directcmd_handler(void *obj, int argc, char *argv[],
		bool *argv_check);
void sftt_client_directcmd_usage(void);

/*
 * Show logged in users' info.
 */
int sftt_client_who_handler(void *obj, int argc, char *argv[],
		bool *argv_check);
void sftt_client_who_usage(void);

/*
 * send a message to another user.
 */
int sftt_client_write_handler(void *obj, int argc, char *argv[],
		bool *argv_check);
void sftt_client_write_usage(void);

/*
 * Sftt client commands supported.
 */
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
		.name = "mps",
		.fn = sftt_client_mps_handler,
		.help = "show the mempool stat both client and server",
		.usage = sftt_client_mps_usage,
	},
	{
		.name = "directcmd",
		.fn = sftt_client_directcmd_handler,
		.help = "enter direct command mode",
		.usage = sftt_client_directcmd_usage,
	},
	{
		.name = "w",
		.fn = sftt_client_who_handler,
		.help = "show logged in users",
		.usage = sftt_client_who_usage,
	},
	{
		.name = "write",
		.fn = sftt_client_write_handler,
		.help = "send a message to another user",
		.usage = sftt_client_write_usage,
	},
	{
		.name = NULL,
		.fn = NULL,
		.help = "",
		.usage = NULL,
	},
};

/*
 * Builtin scripts for testing.
 */
static const struct builtin_script builtin_scripts[] = {
        {
                "mps",
                "show memory pool stat",
                "mps\nmps -d\n"
        },
	{
		NULL,
		NULL,
		NULL
	},
};

#endif
