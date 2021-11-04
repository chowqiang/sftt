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

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <netinet/in.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <curses.h>
#include <ctype.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "base.h"
#include "command.h"
#include "config.h"
#include "context.h"
#include "client.h"
#include "encrypt.h"
#include "endpoint.h"
#include "file.h"
#include "file_trans.h"
#include "log.h"
#include "mkdirp.h"
#include "net_trans.h"
#include "validate.h"
#include "cmdline.h"
#include "packet.h"
#include "debug.h"
#include "req_resp.h"
#include "response.h"
#include "state.h"
#include "trans.h"
#include "user.h"
#include "utils.h"
#include "version.h"

extern int errno;
extern struct mem_pool *g_mp;
extern int verbose_level;

bool directcmd = false;
bool force_quit = false;

struct sftt_option sftt_client_opts[] = {
	{"-u", USER, HAS_ARG},
	{"-h", HOST, HAS_ARG},
	{"-P", PORT, HAS_ARG},
	{"-p", PASSWORD, NO_ARG},
	{NULL, -1, NO_ARG}
};

const char *directcmds[] = {
	"ls",
	"date",
	"cat",
	"mkdir",
	"touch",
	NULL
};

struct dlist *his_cmds;

int consult_block_size_with_server(int sock,
	struct sftt_client_config *client_config);

int send_complete_end_packet(int sock, struct sftt_packet *sp);

struct logged_in_user *find_logged_in_user(struct sftt_client_v2 *client,
		int user_no);

int file_get_next_buffer(struct file_input_stream *fis,
	char *buffer, size_t size)
{
	int ret = fread(buffer, 1, size, fis->fp);		
	
	return ret;
}

int dir_get_next_buffer(struct file_input_stream *fis, char *buffer,
	size_t size)
{
	return 0;
}

int get_cache_port()
{
	FILE *fp = fopen(PORT_CACHE_FILE, "r");
	if (fp == NULL) {
		return -1;
	} 	

	char str[8];
	fgets(str, 8, fp);

	int len = strlen(str);
	if (len < 2) {
		return -1;
	}

	if (str[len - 1] == '\n') {
		str[--len] = 0;
	}

	int port = atoi(str);
	if (port == 0) {
		return -1;
	}

	return port;
}

void set_cache_port(int port)
{
	FILE *fp = fopen(PORT_CACHE_FILE, "w+");
	if (fp == NULL || port > MAX_PORT_NUM) {
		return ;
	}	

	char str[8];
	//itoa(port, str);
	sprintf(str, "%d", port);
	fputs(str, fp);
}

struct file_input_stream *create_file_input_stream(char *file_name)
{
	if (strlen(file_name) > FILE_NAME_MAX_LEN) {
		return NULL;
	}

	struct file_input_stream *fis = (struct file_input_stream *)mp_malloc(
		g_mp, __func__, sizeof(struct file_input_stream));
	if (fis == NULL) {
		return NULL;
	}

	memset(fis->target, 0, FILE_NAME_MAX_LEN);
	strcpy(fis->target, file_name);
	
	if (is_file(fis->target)) {
		fis->get_next_buffer = file_get_next_buffer;
	} else if (is_dir(fis->target)) {
		fis->get_next_buffer = dir_get_next_buffer;
	}

	fis->cursor = 0;
	fis->fp = fopen(fis->target, "r");

	return fis;
}

void destroy_file_input_stream(struct file_input_stream *fis)
{
	if (fis) {
		mp_free(g_mp, fis);
	}
}

int consult_block_size_with_server(int sock,
	struct sftt_client_config *client_config)
{
	unsigned char buffer[BUFFER_SIZE];
		
	memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
	sprintf((char *)buffer, "%d", client_config->block_size);
	//printf("client block size is : %d\n", client_config.block_size);
	sftt_encrypt_func(buffer, BUFFER_SIZE);
	
	int ret = send(sock, buffer, BUFFER_SIZE, 0);
	if (ret <= 0) {
		return -1;
	}
	
	memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
	ret = recv(sock, buffer, BUFFER_SIZE, 0);
	if (ret <= 0) {
		return -1;
	}
	sftt_decrypt_func(buffer, ret);
	int consulted_block_size = atoi((char *)buffer);
	//printf("consulted block size is: %d\n", consulted_block_size);
	
	return consulted_block_size;
}

void usage(char *exec_file)
{
	fprintf (stdout, "\nUsage: %s -h ip <input_file>\n\n", exec_file);
}

int send_complete_end_packet(int sock, struct sftt_packet *sp)
{
	sp->type = PACKET_TYPE_SEND_COMPLETE_REQ;
	sp->data_len = 0;
	int ret = send_sftt_packet(sock, sp);
	if (ret == -1) {
		printf("Error. send complete end block failed!\n");
		return -1;
	}	

	return 0;
}

int find_unfinished_session(struct path_entry *pe, char *ip)
{
	return 0;		
}

int file_trans_session_diff(struct file_trans_session *old_session,
	struct file_trans_session *new_seesion)
{
	return 0;
}

int dir_trans_session_diff(struct dir_trans_session *old_session,
	struct dir_trans_session *new_session)
{
	return 0;
}

int save_trans_session(struct sftt_client *client)
{
	return 0;
}

int check_command_format(char *buf)
{
	return 0;
}

char *parse_exec_name(char *buf, int *offset)
{
	int i = 0, j = 0;
	char exec_name[EXEC_NAME_MAX_LEN];

	while (buf[i] && isspace(buf[i])) {
		++i;
	}

	while ((j < EXEC_NAME_MAX_LEN - 1) && buf[i] && !isspace(buf[i])) {
		exec_name[j++] = buf[i++];
	}
	exec_name[j] = 0;
	*offset = i;

	return __strdup(exec_name);
}

char **parse_args(char *buf, int *argc)
{
	int i = 0;
	struct dlist_node *node = NULL;
	struct dlist *args_list = NULL;
	char arg[CMD_ARG_MAX_LEN];
	int arg_len = 0;
	char **argv = NULL;
	enum cmd_args_state state = INIT;

	args_list = dlist_create(FREE_MODE_NOTHING);
	if (args_list == NULL)
		return NULL;

	for (i = 0; buf[i]; ++i) {
		if (arg_len >= CMD_ARG_MAX_LEN) {
			goto out;
		}
		if (isspace(buf[i])) {
			switch (state) {
			case INIT:
				if (arg_len) {
					arg[arg_len] = 0;
					dlist_append(args_list, __strdup(arg));
					arg_len = 0;
				}
				continue;
			case RECEIVE_SINGLE_QUOTE:
			case AMONG_SINGLE_QUOTE:
			case RECEIVE_DOUBLE_QUOTE:
			case AMONG_DOUBLE_QUOTE:
				arg[arg_len++] = buf[i];
				continue;
			default:
				printf("unexpected state: %d in %s:%d\n",
						state, __FILE__, __LINE__);
				goto out;
			}
		}
		if (buf[i] == '\'') {
			switch (state) {
			case INIT:
				state = RECEIVE_SINGLE_QUOTE;
				continue;
			case RECEIVE_SINGLE_QUOTE:
			case AMONG_SINGLE_QUOTE:
				state = INIT;
				continue;
			default:
				printf("unexpected state: %d in %s:%d\n",
						state, __FILE__, __LINE__);
				goto out;
			}
		}
		if (buf[i] == '"') {
			switch (state) {
			case INIT:
				state = RECEIVE_DOUBLE_QUOTE;
				continue;
			case RECEIVE_DOUBLE_QUOTE:
			case AMONG_DOUBLE_QUOTE:
				state = INIT;
				continue;
			default:
				printf("unexpected state: %d in %s:%d\n",
						state, __FILE__, __LINE__);
				goto out;
			}
		}
		switch (state) {
		case INIT:
			arg[arg_len++] = buf[i];
			break;
		case RECEIVE_SINGLE_QUOTE:
			arg[arg_len++] = buf[i];
			state = AMONG_SINGLE_QUOTE;
			break;
		case AMONG_SINGLE_QUOTE:
			arg[arg_len++] = buf[i];
			break;
		case RECEIVE_DOUBLE_QUOTE:
			arg[arg_len++] = buf[i];
			state = AMONG_DOUBLE_QUOTE;
			break;
		case AMONG_DOUBLE_QUOTE:
			arg[arg_len++] = buf[i];
			break;
		default:
			printf("unexpected state: %d in %s:%d\n",
					state, __FILE__, __LINE__);
			goto out;
		}
	}

	if (state == INIT && arg_len) {
		arg[arg_len] = 0;
		dlist_append(args_list, __strdup(arg));
	}

	*argc = dlist_size(args_list);
	if (*argc) {
		i = 0;
		argv = (char **)mp_malloc(g_mp, __func__, sizeof(char *) * (*argc));
		dlist_for_each(args_list, node) {
			argv[i++] = node->data;
			node->data = NULL;
		}
	}

	dlist_destroy(args_list);
	return argv;

out:
	dlist_destroy(args_list);
	*argc = -1;
	return NULL;
}

struct user_cmd *parse_command(char *buf)
{
	struct user_cmd *cmd;
	int offset;

	if (check_command_format(buf) == -1) {
		return NULL;
	}

	cmd = (struct user_cmd *)mp_malloc(g_mp, __func__, sizeof(struct user_cmd));
	if (!cmd) {
		return NULL;
	}

	cmd->name = parse_exec_name(buf, &offset);
	cmd->argv = parse_args(buf + offset, &cmd->argc);

	if (cmd->argc == -1) {
		if (cmd->name)
			mp_free(g_mp, cmd->name);
		if (cmd->argv)
			mp_free(g_mp, cmd->argv);
		mp_free(g_mp, cmd);
		return NULL;
	}

	return cmd;
}

static int run_command(struct sftt_client_v2 *client,
	const struct cmd_handler *cmd, int argc, char *argv[])
{
	int ret;
	bool argv_check = true;
	
	add_log(LOG_INFO, "running command: %s", cmd->name);
	ret = cmd->fn(client, argc, argv, &argv_check);
	if (!argv_check) {
		cmd->usage();
	}

	return ret;
}

void add_cmd_log(struct user_cmd *cmd)
{
	int i = 0, ret = 0;
	char *buf = mp_malloc(g_mp, __func__, sizeof(char) * 1024);

	if (cmd->argc == 0) {
		ret = sprintf(buf, "exec name: %s, argc: %d", cmd->name, cmd->argc);
	} else {
		ret = sprintf(buf, "exec name: %s, argc: %d, argv: ", cmd->name, cmd->argc);
		for (i = 0; i < cmd->argc - 1; ++i) {
			ret += sprintf(buf + ret, "%s, ", cmd->argv[i]);
		}
		ret += sprintf(buf + ret, "%s", cmd->argv[i]);
	}
	buf[ret] = 0;

	add_log(LOG_INFO, "%s", buf);
	mp_free(g_mp, buf);
}

int execute_directcmd(struct sftt_client_v2 *client, char *buf)
{
	struct sftt_packet *req_packet = NULL;
	struct sftt_packet *resp_packet = NULL;
	struct directcmd_req *req_info = NULL;
	struct directcmd_resp *resp_info = NULL;
	long recv_len = 0, total_len = 0;
	int ret = 0;

	if (strlen(buf) == 0)
		return -1;

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_DIRECTCMD_REQ;

	req_info = mp_malloc(g_mp, "directcmd_handler_req", sizeof(struct directcmd_req));
	if (req_info == NULL) {
		printf("alloc directcmd_handler_req failed!\n");
		ret = -1;
		goto done;
	}

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	strncpy(req_info->cmd, buf, CMD_MAX_LEN - 1);

	req_packet->obj = req_info;
	ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		goto done;
	}

	resp_packet = malloc_sftt_packet();
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		ret = -1;
		goto done;
	}

	ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		ret = -1;
		goto done;
	}

	resp_info = (struct directcmd_resp *)resp_packet->obj;
	assert(resp_info != NULL);

	if (resp_info->data.total_len < 0 || resp_info->data.this_len < 0) {
		printf("command execute failed!\n");
		ret = -1;
		goto done;
	}

	total_len = resp_info->data.total_len;

	do {
		printf("%s", resp_info->data.content);

		recv_len += resp_info->data.this_len;
		mp_free(g_mp, resp_info);

		if (recv_len == total_len)
			break;

		ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}

		resp_info = (struct directcmd_resp *)resp_packet->obj;
		assert(resp_info != NULL);

	} while (recv_len < total_len);

done:
	if (req_info)
		mp_free(g_mp, req_info);

	if (req_packet)
		free_sftt_packet(&req_packet);

	if (resp_packet)
		free_sftt_packet(&resp_packet);

	return ret;
}

bool among_directcmds(char *name)
{
	int i = 0;

	for (i = 0; directcmds[i]; ++i) {
		if (strcmp(name, directcmds[i]) == 0)
			return true;
	}

	return false;
}

int execute_cmd(struct sftt_client_v2 *client, char *buf, int flag)
{
	int i = 0, ret = 0;
	bool found = false;
	struct user_cmd *cmd = NULL;

	add_log(LOG_INFO, "input command: %s", buf);

	cmd = parse_command(buf);
	if (!cmd) {
		printf("sftt client: cannot find command: %s\n"
			"please input 'help' to get the usage.\n", buf);
		return -1;
	}

	if (directcmd && among_directcmds(cmd->name))
		return execute_directcmd(client, buf);

	add_cmd_log(cmd);
	for (i = 0; sftt_client_cmds[i].name != NULL; ++i) {
		if (!strcmp(cmd->name, sftt_client_cmds[i].name)) {
			found = true;
			ret = run_command(client, &sftt_client_cmds[i], cmd->argc, cmd->argv);
			break;
		}
	}

	if (!found)
		printf("sftt client: cannot find command: %s\n"
			"please input 'help' to get the usage.\n", cmd->name);

	if (cmd->name)
		mp_free(g_mp, cmd->name);

	for (i = 0; i < cmd->argc; ++i) {
		mp_free(g_mp, cmd->argv[i]);
	}

	if (cmd->argv)
		mp_free(g_mp, cmd->argv);

	mp_free(g_mp, cmd);

	return ret;
}

bool parse_user_name(char *optarg, char *user_name, int max_len)
{
	int len = strlen(optarg);
	if (!(0 < len && len <= max_len )) {
		return false;
	}	
	strcpy(user_name, optarg);

	return true;
}

bool parse_host(char *optarg, char *host, int max_len)
{
	int len = strlen(optarg);
	if (!(0 < len && len <= max_len)) {
		return false;
	}
	strcpy(host, optarg);

	return true;
}

bool parse_port(char *optarg, int *port)
{
	if (!isdigit(optarg)) {
		return false;
	}

	*port = atoi(optarg);

	return true;
}

void client_usage_help(int exitcode)
{
	show_version();
	printf("usage:\t" PROC_NAME " [-u user] [-h host] [-p port] [-v]\n"
		"\t" PROC_NAME " -u root -h localhost [-p port]\n");
	exit(exitcode);
}

static int init_sftt_client_ctrl_conn(struct sftt_client_v2 *client, int port)
{
	if (port == -1) {
		port = get_random_port();
	}

	if (verbose_level)
		printf("port of connecting: %d\n", port);

	client->main_conn.sock = make_connect(client->host, port);
	if (client->main_conn.sock == -1) {
		return -1;
	}

	client->main_conn.type = CONN_TYPE_CTRL;
	client->main_conn.port = port;

	return 0;
}

static int validate_user_base_info(struct sftt_client_v2 *client, char *passwd)
{
	struct sftt_packet *req_packet = NULL;
	struct sftt_packet *resp_packet = NULL;
	struct validate_req *req_info = NULL;
	struct validate_resp *resp_info = NULL;
	struct validate_resp_data *data = NULL;
	int ret = 0;

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_VALIDATE_REQ;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct validate_req));
	if (req_info == NULL) {
		printf("alloc req_info failed!\n");
		ret = -1;
		goto done;
	}

	req_info->ver = client->ver;
	strncpy(req_info->name, client->uinfo.name, USER_NAME_MAX_LEN - 1);
	req_info->name_len = strlen(req_info->name);

	if (strlen(passwd)) {
		md5_str((unsigned char *)passwd, strlen(passwd),
			(unsigned char *)req_info->passwd_md5);
	} else {
		req_info->passwd_md5[0] = 0;
	}
	req_info->passwd_len = strlen(req_info->passwd_md5);

	req_packet->obj = req_info;
	ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		goto done;
	}

	resp_packet = malloc_sftt_packet();
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		ret = -1;
		goto done;
	}

	ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		goto done;
	}

	resp_info = (validate_resp *)resp_packet->obj;
	data = &resp_info->data;
	if (resp_info->status != RESP_UVS_PASS) {
		printf("%s: validate status is not pass! status: 0x%0x\n",
			__func__, resp_info->status);
		switch (resp_info->status) {
		case RESP_UVS_NTFD:
			printf("user %s not found!\n", data->name);
			break;
		case RESP_UVS_INVALID:
			printf("user name and passwd not match!\n");
			break;
		case RESP_UVS_MISSHOME:
			printf("user %s's home dir cannot access!\n",
				data->name);
			break;
		case RESP_UVS_BLOCK:
			printf("user %s blocked!\n", data->name);
			break;
		case RESP_UVS_BAD_VER:
			printf("%s\n", resp_info->message);
			break;
		default:
			printf("validate exception!\n");
			break;
		}
		goto done;
	}

	client->uinfo.uid = data->uid;
	add_log(LOG_INFO, "uid: %d", client->uinfo.uid);

	strncpy(client->session_id, data->session_id, SESSION_ID_LEN - 1);
	strncpy(client->pwd, data->pwd, DIR_PATH_MAX_LEN - 1);

done:

	if (req_info)
		mp_free(g_mp, req_info);

	if (resp_info)
		mp_free(g_mp, resp_info);

	if (req_packet)
		free_sftt_packet(&req_packet);

	if (resp_packet)
		free_sftt_packet(&resp_packet);

	return ret;
}

static int init_sftt_client_session(struct sftt_client_v2 *client)
{
	return 0;
}

struct peer_task *create_peer_task(struct peer_task_handler *handler)
{
	struct peer_task *task;

	task = mp_malloc(g_mp, "peer_task", sizeof(struct peer_task));
	if (task == NULL)
		return NULL;

	INIT_LIST_HEAD(&task->list);

	handler->pm->ops->lock(handler->pm);
	if (handler->tasks == NULL) {
		handler->tasks = task;
	} else {
		list_add_tail(&task->list, &handler->tasks->list);
	}

	handler->pm->ops->unlock(handler->pm);

	return task;
}

int handle_peer_write_req(struct client_sock_conn *conn, struct sftt_packet *req_packet,
		struct sftt_packet *resp_packet)
{
	struct write_req *req = req_packet->obj;

	printf("%s\n", req->message);

	return 0;
}

int handle_peer_ll_req(struct client_sock_conn *conn, struct sftt_packet *req_packet,
		struct sftt_packet *resp_packet)
{
	struct ll_req *req = NULL;
	struct ll_resp *resp = NULL;
	struct ll_resp_data *data;
	char tmp[2 * DIR_PATH_MAX_LEN + 4];
	char path[2 * DIR_PATH_MAX_LEN + 2];
	int i = 0, ret = 0;
	struct dlist *file_list;
	struct dlist_node *node;
	bool has_more = false;

	DEBUG((DEBUG_INFO, "handle ll req in ...\n"));

	req = req_packet->obj;
	assert(req != NULL);

	DEBUG((DEBUG_INFO, "ll_req|path=%s\n", req->path));

	resp = mp_malloc(g_mp, __func__, sizeof(struct ll_resp));
	if (resp == NULL) {
		printf("alloc ll_resp failed!\n");
		return -1;
	}

	strncpy(path, req->path, FILE_NAME_MAX_LEN - 1);
	simplify_path(path);
	DEBUG((DEBUG_INFO, "ls -l|path=%s\n", path));

	data = &resp->data;
	if (!file_existed(path)) {
		data->total = -1;
		DEBUG((DEBUG_INFO, "file not existed!\n"));
		ret = send_ll_resp(conn->sock, resp_packet,
			       resp, RESP_FILE_NTFD, 0);
		goto done;
	}

	if (is_file(path)) {
		data->total = 1;

		strncpy(data->entries[0].name, basename(path), FILE_NAME_MAX_LEN - 1);
		data->entries[0].type = FILE_TYPE_FILE;

		DEBUG((DEBUG_INFO, "list file successfully!\n"));

		ret = send_ll_resp(conn->sock, resp_packet,
				resp, RESP_OK, 0);
		goto done;
	} else if (is_dir(path)) {
		file_list = get_top_file_list(path);
		if (file_list == NULL) {
			data->total = -1;
			ret = send_ll_resp(conn->sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0);
			if (ret == -1) {
				DEBUG((DEBUG_INFO, "send ll resp failed!\n"));
			}

			goto done;
		}

		DEBUG((DEBUG_INFO, "list dir ...\n"));
		DEBUG((DEBUG_INFO, "file_count=%d\n", dlist_size(file_list)));
		data->total = dlist_size(file_list);

		node = dlist_head(file_list);
		while (node) {
			i = 0;
			dlist_for_each_pos(node) {
				if (i == FILE_ENTRY_MAX_CNT)
					break;
				strncpy(data->entries[i].name, node->data, FILE_NAME_MAX_LEN - 1);
				snprintf(tmp, sizeof(tmp) - 1, "%s/%s", path, (char *)node->data);
				simplify_path(tmp);
				DEBUG((DEBUG_INFO, "file_name=%s\n", tmp));

				if (is_file(tmp))
					data->entries[i].type = FILE_TYPE_FILE;
				else if (is_dir(tmp))
					data->entries[i].type = FILE_TYPE_DIR;
				else
					data->entries[i].type = FILE_TYPE_UNKNOWN;
				++i;
			}
			data->this_nr = i;
			if (node) {
				has_more = true;
				ret = send_ll_resp(conn->sock, resp_packet,
					resp, RESP_OK, 1);
				if (ret == -1) {
					DEBUG((DEBUG_INFO, "send ll resp failed!\n"));
				}
			} else {
				has_more = false;
				ret = send_ll_resp(conn->sock, resp_packet,
					resp, RESP_OK, 0);
				if (ret == -1) {
					DEBUG((DEBUG_INFO, "send ll resp failed!\n"));
				}
			}
		}
	}

	if (has_more) {
		ret = send_ll_resp(conn->sock, resp_packet,
			resp, RESP_OK, 0);
		if (ret == -1) {
			DEBUG((DEBUG_INFO, "send ll resp failed!\n"));
		}
	}

	DEBUG((DEBUG_INFO, "list file successfully!\n"));
	DEBUG((DEBUG_INFO, "handle ll req out\n"));
done:
	if (resp)
		mp_free(g_mp, resp);

	return ret;
}

int handle_peer_get_req(struct client_sock_conn *conn, struct sftt_packet *req_packet,
		struct sftt_packet *resp_packet)
{
	struct get_req *req;
	struct get_resp *resp;
	char path[FILE_NAME_MAX_LEN];
	int ret = 0;

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "handle get req in ...\n"));

	req = req_packet->obj;
	strncpy(path, req->path, FILE_NAME_MAX_LEN);

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "get req|session_id=%s|path=%s\n",
				req->session_id, req->path));

	resp = mp_malloc(g_mp, __func__, sizeof(struct get_resp));
	if (resp == NULL) {
		printf("alloc get_resp failed!\n");
		return -1;
	}

	if (!is_absolute_path(path)) {
		DEBUG((DEBUG_INFO, "path not absolute!\n"));
		send_get_resp(conn->sock, resp_packet, resp,
				RESP_PATH_NOT_ABS, 0);
		ret = -1;
		goto done;
	}

	if (!file_existed(path)) {
		DEBUG((DEBUG_INFO, "file not existed!\n"));
		send_get_resp(conn->sock, resp_packet, resp,
				RESP_FILE_NTFD, 0);
		ret = -1;
		goto done;
	}

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "begin to send files by get resp"));

	ret = send_files_by_get_resp(conn->sock, path, resp_packet,
			resp);

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "handle get req out\n"));

done:
	if (resp)
		mp_free(g_mp, resp);

	return ret;
}

int handle_peer_put_req(struct client_sock_conn *conn, struct sftt_packet *req_packet,
		struct sftt_packet *resp_packet)
{
	int ret;

	DEBUG((DEBUG_INFO, "handle put req in ...\n"));
	ret = recv_files_by_put_req(conn->sock, req_packet);
	DEBUG((DEBUG_INFO, "handle put req out\n"));

	return ret;
}

void clean_task(struct peer_task_handler *handler)
{

}

int get_friend_list(struct sftt_client_v2 *client)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct who_req *req_info;
	struct who_resp *resp_info;
	struct friend_user *friend;
	struct who_resp_data *data;
	int ret, i = 0, total = 0;

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_WHO_REQ;

	req_info = mp_malloc(g_mp, "get_friend_list_req", sizeof(struct who_req));
	if (req_info == NULL) {
		printf("alloc get_friend_list_req failed!\n");
		return -1;
	}

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);

	req_packet->obj = req_info;

	ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet();
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (struct who_resp *)resp_packet->obj;
	assert(resp_info != NULL);

	data = &resp_info->data;
	while (data->total > 0 && data->this_nr > 0) {
		for (i = 0; i < data->this_nr; ++i) {
			friend = mp_malloc(g_mp, "get_friend_list_resp_friend",
					sizeof(struct friend_user));
			if (friend == NULL) {
				printf("alloc get_friend_list_resp failed!\n");
				return -1;
			}
			friend->info = data->users[i];

			list_add_tail(&friend->list, &client->friends);
		}

		total += data->this_nr;
		if (total == data->total)
			break;

		ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			return -1;
		}

		resp_info = (struct who_resp *)resp_packet->obj;
		assert(resp_info != NULL);

		data = &resp_info->data;
	}

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return 0;
}

void clear_friend_list(struct sftt_client_v2 *client)
{
	struct friend_user *p, *q;

	list_for_each_entry_safe(p, q, &client->friends, list) {
		list_del(&p->list);
		mp_free(g_mp, p);
	}
}

int init_friend_list(struct sftt_client_v2 *client)
{
	INIT_LIST_HEAD(&client->friends);

	return 0;
}

void *do_task_handler(void *arg)
{
	struct client_sock_conn *conn = arg;
	int sock = conn->sock;
	struct sftt_packet *resp;
	struct sftt_packet *req;
	int ret;

	req = malloc_sftt_packet();
	if (!req) {
		printf("cannot allocate resources from memory pool!\n");
		return NULL;
	}

	resp = malloc_sftt_packet();
	if (resp == NULL) {
		goto exit;
	}

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "begin to communicate with peer ...\n"));

	while (1) {
		conn->is_using = false;

		ret = recv_sftt_packet(sock, req);
		add_log(LOG_INFO, "recv ret: %d", ret);
		if (ret == -1) {
			printf("recv encountered unrecoverable error, child process is exiting ...\n");
			goto exit;
		}
		if (ret == 0) {
			add_log(LOG_INFO, "client disconnected, child process is exiting ...");
			goto exit;
		}

		conn->is_using = true;
		switch (req->type) {
		case PACKET_TYPE_WRITE_REQ:
			handle_peer_write_req(conn, req, resp);
			break;
		case PACKET_TYPE_LL_REQ:
			handle_peer_ll_req(conn, req, resp);
			break;
		case PACKET_TYPE_GET_REQ:
			handle_peer_get_req(conn, req, resp);
			break;
		case PACKET_TYPE_PUT_REQ:
			handle_peer_put_req(conn, req, resp);
			break;
		default:
			break;
		}
		free_sftt_packet(&resp);
	}

exit:
	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "a client is disconnected\n"));

	return NULL;
}

int start_task_handler(struct sftt_client_v2 *client, struct client_sock_conn *conn)
{
	int ret;

	ret = pthread_create(&conn->tinfo.tid, NULL, do_task_handler, conn);
	if (ret == -1) {
		printf("create task handler thread failed!\n");
		return -1;
	}

	return 0;
}

int add_task_connect(struct sftt_client_v2 *client)
{
	int port;
	struct client_sock_conn *conn;
	struct sftt_packet *req_packet, *resp_packet;
	struct append_conn_req *req_info;
	struct append_conn_resp *resp_info;
	int ret, i = 0, total = 0;

	conn = mp_malloc(g_mp, "client_sock_conn", sizeof(struct client_sock_conn));
	if (conn == NULL) {
		printf("alloc client_sock_conn failed!\n");
		return -1;
	}

	port = get_random_port();
	conn->sock = make_connect(client->host, port);
	if (conn->sock == -1) {
		printf("make client sock connect failed!\n");
		return -1;
	}
	conn->port = port;

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_APPEND_CONN_REQ;

	req_info = mp_malloc(g_mp, "append_conn_req", sizeof(struct append_conn_req));
	if (req_info == NULL) {
		printf("alloc append_conn_req failed!\n");
		return -1;
	}

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	req_info->type = CONN_TYPE_TASK;

	req_packet->obj = req_info;
	//req_packet->block_size = APPEND_CONN_REQ_PACKET_MIN_LEN;

	ret = send_sftt_packet(conn->sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet();
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(conn->sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (struct append_conn_resp *)resp_packet->obj;
	assert(resp_info != NULL);

	if (resp_info->status != RESP_OK) {
		printf("append conn failed: %s\n", resp_info->message);
		return -1;
	}

	conn->type = CONN_TYPE_TASK;
	strncpy(conn->connect_id, resp_info->data.connect_id, CONNECT_ID_LEN);

	client->tcs_lock->ops->lock(client->tcs_lock);
	list_add(&conn->list, &client->task_conns);
	client->tcs_lock->ops->unlock(client->tcs_lock);

	ret = start_task_handler(client, conn);
	if (ret == -1) {
		printf("start task handle failed!\n");
		return -1;
	}

	return 0;
}

void *do_connect_manager(void *arg)
{
	int idle_conns = 0;
	struct sftt_client_v2 *client = arg;
	struct client_sock_conn *conn;
	int conns = 0;
	int ret;

	while (!force_quit) {
		conns = 0;
		idle_conns = 0;

		client->tcs_lock->ops->lock(client->tcs_lock);
		list_for_each_entry(conn, &client->task_conns, list) {
			if (!conn->is_using)
				idle_conns++;
			conns++;
		}
		client->tcs_lock->ops->unlock(client->tcs_lock);

		if (conns < CLIENT_MAX_TASK_CONN && idle_conns == 0) {
			ret = add_task_connect(client);
			if (ret == -1) {
				printf("add task connect failed!\n");
			}
		}

		sleep(1);
	}
	
	return NULL;
}

int start_conn_mgr(struct sftt_client_v2 *client)
{
	int ret;

	INIT_LIST_HEAD(&client->task_conns);

	client->tcs_lock = new(pthread_mutex);
	if (client->tcs_lock == NULL) {
		printf("create tcs_lock failed!\n");
		return -1;
	}

	ret = pthread_create(&client->conn_mgr.tid, NULL, do_connect_manager,
			client);
	if (ret == -1) {
		perror("create conn_mgr failed");
		return -1;
	}

	return 0;
}

int init_sftt_client_v2(struct sftt_client_v2 *client, char *host, int port,
	char *user, char *passwd)
{
	/*
	 * set client context globally
	 */
	set_current_context("client");
	set_log_type(CLIENT_LOG);

	if (get_version_info(&client->ver) == -1) {
		printf("get sftt client version info failed!\n");
		return -1;
	}

	strncpy(client->host, host, HOST_MAX_LEN - 1);

	client->mp = get_singleton_mp();
	strncpy(client->uinfo.name, user, USER_NAME_MAX_LEN - 1);

	if (get_sftt_client_config(&client->config) == -1) {
		printf("get sftt client config failed!\n");
		return -1;
	}

	if (logger_init(client->config.log_dir, PROC_NAME) == -1) {
		printf("init logger failed!\n");
	}

	if (init_sftt_client_ctrl_conn(client, port) == -1) {
		printf("init sftt client control connection failed!\n");
		return -1;
	}

	if (init_sftt_client_session(client) == -1) {
		printf("init sftt client session failed!\n");
		return -1;
	}

	if (validate_user_base_info(client, passwd) == -1) {
		printf("cannot validate user and password!\n");
		return -1;
	}

	if (start_conn_mgr(client) == -1) {
		printf("start task connects manager failed!\n");	
	}

	if (init_friend_list(client) == -1) {
		printf("cannot init friend list!\n");
		return -1;
	}

	if (get_friend_list(client) == -1) {
		printf("cannot get friend list!\n");
		return -1;
	}

	return 0;
}

int show_options(char *host, char *user_name, char *password)
{
	add_log(LOG_INFO, "host: %s", host);
	add_log(LOG_INFO, "your name: %s", user_name);
	add_log(LOG_INFO, "your password: %s", password);

	return 0;
}

void sftt_client_ll_usage(void)
{
	printf("Usage: ll [user_no] [path]\n");
}

int sftt_client_ll_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct ll_req *req_info;
	struct ll_resp *resp_info;
	struct ll_resp_data *data;
	struct sftt_client_v2 *client = obj;
	struct dlist *fe_list;
	struct file_entry *entry;
	struct dlist_node *node;
	struct logged_in_user *user = NULL;
	char *path = NULL;
	int ret, i = 0, total = 0;

	if (argc > 2 || argc < 0) {
		sftt_client_ll_usage();
		return -1;
	}

	if (argc == 0)
		path = client->pwd;
	else if (argc == 1)
		path = argv[0];
	else {
		user = find_logged_in_user(client, atoi(argv[0]));
		if (user == NULL) {
			printf("cannot find logged in user for %s\n", argv[0]);
			return -1;
		}
		path = argv[1];
	}

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_LL_REQ;

	req_info = mp_malloc(g_mp, "ll_handler_req", sizeof(struct ll_req));
	if (req_info == NULL) {
		printf("alloc ll_handler_req failed!\n");
		return -1;
	}

	if (user) {
		req_info->to_peer = 1;
		req_info->user = *user;
	} else {
		req_info->to_peer = 0;
	}

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	strncpy(req_info->path, path, DIR_PATH_MAX_LEN - 1);

	req_packet->obj = req_info;
	//req_packet->block_size = LL_REQ_PACKET_MIN_LEN;

	ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet();
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	fe_list = dlist_create(FREE_MODE_MP_FREE);
	if (fe_list == NULL) {
		printf("create dlist failed!\n");
		return -1;
	}

	resp_info = (struct ll_resp *)resp_packet->obj;
	assert(resp_info != NULL);

	data = &resp_info->data;
	while (data->total > 0 && data->this_nr > 0) {
		for (i = 0; i < data->this_nr; ++i) {
			entry = mp_malloc(g_mp, "ll_handler_resp_file_entry",
					sizeof(struct file_entry));
			if (entry == NULL) {
				printf("alloc ll_handler_resp_file_entry failed!\n");
				return -1;
			}
			*entry = data->entries[i];
			dlist_append(fe_list, entry);
		}

		total += data->this_nr;
		if (total == data->total)
			break;

		ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			return -1;
		}

		resp_info = (struct ll_resp *)resp_packet->obj;
		assert(resp_info != NULL);

		data = &resp_info->data;
	}

	dlist_for_each(fe_list, node) {
		entry = (struct file_entry *)node->data;
		printf("%s\t%s\n", FILE_TYPE_NAME(entry->type), entry->name);
	}

	dlist_destroy(fe_list);

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return 0;
}

int sftt_client_help_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	*argv_check = true;

	printf("sftt client commands:\n\n"
		"\tcd         change work directory\n"
		"\tdirectcmd  enter direct command mode\n"
		"\tget        get file(s) from server\n"
		"\thelp       show sftt client help info\n"
		"\this        show history command\n"
		"\tll         list directory contents using a long listing format\n"
		"\tmps        show the mempool stat both client and server\n"
		"\tput        put file(s) to server\n"
		"\tpwd        get current directory\n"
		"\tw          show logged in user(s)\n"
		"\twrite      send a message to another user\n"
	      );

	return 0;
}

int sftt_client_his_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	int i = 0, num = 10;
	HIST_ENTRY **his;

	if (argc > 0) {
		num = atoi(argv[0]);
		add_log(LOG_INFO, "his number specified: %d", num);
	}

	his = history_list();
	if (his == NULL)
		return 0;

	while (i < num && his[i] && his[i]->line) {
		printf("%s\n", his[i]->line);
		++i;
	}

	return 0;
}

void sftt_client_his_usage(void)
{
	printf("Usage: his\n");
}

void sftt_client_help_usage(void)
{
	printf("Usage: help\n");
}

int sftt_client_cd_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct cd_req *req_info;
	struct cd_resp *resp_info;
	struct sftt_client_v2 *client = obj;
	struct cd_resp_data *data;

	if (argc != 1 || strlen(argv[0]) == 0) {
		sftt_client_cd_usage();
		return -1;
	}

	req_info = mp_malloc(g_mp, __func__, sizeof(struct cd_req));
	if (req_info == NULL) {
		printf("alloc cd_req failed!\n");
		return -1;
	}

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	strncpy(req_info->path, argv[0], DIR_PATH_MAX_LEN);

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_CD_REQ;

	req_packet->obj = req_info;
	//req_packet->block_size = CD_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet();
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (struct cd_resp *)resp_packet->obj;
	if (resp_info->status != RESP_OK) {
		printf("change directory failed!\n");
		ret = -1;
	} else {
		data = &resp_info->data;
		printf("pwd change to: %s\n", data->pwd);
		strncpy(client->pwd, data->pwd, DIR_PATH_MAX_LEN - 1);
		ret = 0;
	}

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return ret;
}

void sftt_client_cd_usage(void)
{
	printf("Usage: cd path\n");
}

int sftt_client_pwd_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct pwd_req *req_info;
	struct pwd_resp *resp_info;
	struct sftt_client_v2 *client = obj;
	struct pwd_resp_data *data;

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_PWD_REQ;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct pwd_req));
	if (req_info == NULL) {
		printf("alloc pwd_req failed!\n");
		return -1;
	}

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);

	req_packet->obj = req_info;
	//req_packet->block_size = PWD_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet();
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (pwd_resp *)resp_packet->obj;
	data = &resp_info->data;
	printf("pwd: %s\n", data->pwd);

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return 0;
}

void sftt_client_pwd_usage(void)
{
	printf("Usage: pwd\n");
}

int sftt_client_get_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct get_req *req;
	struct sftt_packet *req_packet;
	struct sftt_packet *resp_packet;
	struct sftt_client_v2 *client = obj;
	int ret, user_no;
	char *target = NULL;
	char *path = NULL;
	struct logged_in_user *user = NULL;

	if (!(argc == 2 || argc == 3)) {
		sftt_client_get_usage();
		return -1;
	}

	if (argc == 2) {
		path = argv[0];
		target = argv[1];
	} else if (argc == 3) {
		user_no = atoi(argv[0]);
		user = find_logged_in_user(client, user_no);
		if (user == NULL) {
			printf("cannot find user %d, please check the user by using"
					" command \"w\"\n", user_no);
			return -1;
		}

		path = argv[1];
		target = argv[2];
	}

	if (!is_absolute_path(path)) {
		printf("you must specify the absolute path when get"
			" from peer!\n");
		return -1;
	}

	req_packet = malloc_sftt_packet();
	if (req_packet == NULL) {
		printf("%s: malloc sftt paceket failed!\n", __func__);
		return -1;
	}


	req = mp_malloc(g_mp, "get_handler_req", sizeof(struct get_req));
	if (req == NULL) {
		printf("%s: malloc get req failed!\n", __func__);
		return -1;
	}

	/* send get req */
	strncpy(req->session_id, client->session_id, SESSION_ID_LEN);
	strncpy(req->path, path, FILE_NAME_MAX_LEN);
	if (user) {
		req->user = *user;
		req->to_peer = 1;
	}

	req_packet->obj = req;
	req_packet->type = PACKET_TYPE_GET_REQ;

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "send get req to server\n"));

	ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet();
	if (resp_packet == NULL) {
		printf("%s: malloc sftt paceket failed!\n", __func__);
		return -1;
	}

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "begin to recv files from server\n"));

	ret = recv_files_from_get_resp(client->main_conn.sock, target, resp_packet);

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "end to recv files from server\n"));

	mp_free(g_mp, req);

	free_sftt_packet(&req_packet);

	return ret;
}

void sftt_client_get_usage(void)
{
	printf("Usage: get [user_no] file|dir file|dir\n");
}

int sftt_client_put_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct sftt_client_v2 *client = obj;
	FILE *fp;
	int i = 0, ret;
	struct dlist *file_list;
	struct dlist_node *node;
	int file_count;
	char file[FILE_NAME_MAX_LEN];
	char target[FILE_NAME_MAX_LEN + 2];
	struct path_entry *entry;
	struct sftt_packet *req_packet;
	struct logged_in_user *user;
	struct put_req *req;
	int user_no;

	if (!(argc == 2 || argc == 3)) {
		sftt_client_put_usage();
		return -1;
	}

	user = NULL;
	if (argc == 2) {
		strncpy(file, argv[0], FILE_NAME_MAX_LEN - 1);
		if (!is_absolute_path(argv[1])) {
			snprintf(target, sizeof(target), "%s/%s",
					client->pwd, argv[1]);
		} else {
			strncpy(target, argv[1], FILE_NAME_MAX_LEN - 1);
		}
	} else if (argc == 3) {
		user_no = atoi(argv[0]);
		user = find_logged_in_user(client, user_no);
		if (user == NULL) {
			printf("cannot find user %d, please update the user"
					" list by using command \"w\"\n", user_no);
			return -1;
		}
		strncpy(file, argv[1], FILE_NAME_MAX_LEN - 1);
		if (!is_absolute_path(argv[2])) {
			printf("when you put files to peer, please specify the"
					" absolute path on peer\n");
			return -1;
		}
		strncpy(target, argv[2], FILE_NAME_MAX_LEN - 1);
	} else {
		sftt_client_put_usage();
		return -1;
	}

	if (!is_file(file) && !is_dir(file)) {
		printf("cannot access: %s\n", file);
		return -1;
	}

	req_packet = malloc_sftt_packet();
	if (req_packet == NULL) {
		printf("%s:%d, alloc req packet failed!\n", __func__, __LINE__);
		return -1;
	}

	req = mp_malloc(g_mp, __func__, sizeof(struct put_req));
	if (req == NULL) {
		printf("%s:%d, alloc put_req failed!\n", __func__, __LINE__);
		return -1;
	}

	req->to_peer = user ? 1 : 0;
	if (req->to_peer)
		req->user = *user;

	ret = send_files_by_put_req(client->main_conn.sock, file, target, req_packet, req);
	if (ret == -1) {
		printf("%s:%d, handle put req failed!\n", __func__, __LINE__);
	}

	free_sftt_packet(&req_packet);

	return ret;
}

void sftt_client_put_usage(void)
{
	printf("Usage: put [user_no] file|dir file|dir\n");
}

int sftt_client_mps_detail(void *obj)
{
	int i = 0;
	struct mem_pool_using_detail *detail;
	struct mem_pool_stat stat;
	struct using_node *node;

	detail = get_mp_stat_detail(g_mp);
	if (detail == NULL) {
		printf("internal error!\n");
		return -1;
	}

	printf("<%s, %s>\n", "purpose", "using_nodes");
	for (i = 0; i < detail->node_count; ++i) {
		node = &detail->nodes[i];
		printf("<%s, %d>\n", node->purpose, node->count);
	}
	printf("\n");

	get_mp_stat(g_mp, &stat);

	printf("\ttotal_size\ttotal_nodes\tusing_nodes\tfree_nodes\n");
	printf("client\t%ld\t\t%d\t\t%d\t\t%d\n\n", stat.total_size,
		stat.total_nodes, stat.using_nodes, stat.free_nodes);


	return 0;
}

void sftt_client_directcmd_usage(void)
{
	printf("Usage: directcmd in|out\n");
}

int sftt_client_directcmd_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	int i = 0;
	struct sftt_client_v2 *client = obj;

	if (argc != 1) {
		sftt_client_directcmd_usage();
		return -1;
	}

	if (strcmp(argv[0], "in") && strcmp(argv[0], "out")) {
		sftt_client_directcmd_usage();
		return -1;
	}

	if (strcmp(argv[0], "in") == 0) {
		directcmd = true;
		printf("enter direct command mode ...\n");
	} else {
		directcmd = false;
		printf("exit direct command mode ...\n");
	}

	return 0;
}

int sftt_client_mps_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct mp_stat_req *req_info;
	struct mp_stat_resp *resp_info;
	struct sftt_client_v2 *client = obj;
	struct mem_pool_stat stat;
	struct mp_stat_resp_data *data;

	if (argc > 1) {
		sftt_client_mps_usage();
		return -1;
	}

	if (argc == 1) {
		if (strcmp(argv[0], "-d")) {
			sftt_client_mps_usage();
			return -1;
		}
		return sftt_client_mps_detail(obj);
	}

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_MP_STAT_REQ;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct mp_stat_req));
	if (req_info == NULL) {
		printf("alloc mp_stat_req failed!\n");
		return -1;
	}

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);

	req_packet->obj = req_info;
	//req_packet->block_size = MP_STAT_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet();
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (struct mp_stat_resp *)resp_packet->obj;
	get_mp_stat(g_mp, &stat);

	data = &resp_info->data;

	printf("\ttotal_size\ttotal_nodes\tusing_nodes\tfree_nodes\n");
	printf("client\t%ld\t\t%d\t\t%d\t\t%d\n", stat.total_size,
		stat.total_nodes, stat.using_nodes, stat.free_nodes);
	printf("server\t%ld\t\t%d\t\t%d\t\t%d\n", data->total_size,
		data->total_nodes, data->using_nodes, data->free_nodes);

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return 0;

}

void sftt_client_mps_usage(void)
{
	printf("Usage: mps [-d]\n");
}

void sftt_client_write_usage(void)
{
	printf("Usage: write user_no \"message\"\n");
}

struct logged_in_user *find_logged_in_user(struct sftt_client_v2 *client,
		int user_no)
{
	struct friend_user *p;
	int i = 0;

	list_for_each_entry(p, &client->friends, list)
		if (i++ == user_no)
			return &p->info;

	return NULL;
}

int sftt_client_write_handler(void *obj, int argc, char *argv[],
		bool *argv_check)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct write_req *req_info;
	struct write_resp *resp_info;
	struct sftt_client_v2 *client = obj;
	struct logged_in_user *user;
	int user_no;

	if (argc != 2) {
		sftt_client_write_usage();
		return -1;
	}

	req_packet = malloc_sftt_packet();
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_WRITE_REQ;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct write_req));
	if (req_info == NULL) {
		printf("alloc write_req failed!\n");
		return -1;
	}

	user_no = atoi(argv[0]);
	user = find_logged_in_user(client, user_no);
	if (user == NULL) {
		printf("cannot find user %d, please check the user by using"
				" command \"w\"\n", user_no);
		return -1;
	}

	req_info->user = *user;
	strncpy(req_info->message, argv[1], WRITE_MSG_MAX_LEN - 1);
	req_info->len = strlen(req_info->message);

#if 0
	printf("req_info->user->session_id=%s\n", req_info->user.session_id);
	printf("req_info->user->ip=%s\n", req_info->user.ip);
	printf("req_info->user->port=%d\n", req_info->user.port);
	printf("req_info->user->name=%s\n", req_info->user.name);
#endif
	req_packet->obj = req_info;
	//req_packet->block_size = WRITE_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->main_conn.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

#if 0
	resp_packet = malloc_sftt_packet(WRITE_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (struct write_resp *)resp_packet->obj;

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);
#endif

	return 0;
}


/* Read and execute commands until user inputs 'quit' command */
int reader_loop(struct sftt_client_v2 *client)
{
	int start;
	struct cmd_line cmd;
	struct dlist_node *last = NULL, *next = NULL;
	his_cmds = dlist_create(FREE_MODE_FREE);

	start = 0;
	cmd.buf[0] = 0;
	while (1) {
		get_user_command("sftt>> ", &cmd, start);
		if (cmd.type == CMD_LINE_ARROW) {
			if (cmd.buf[0] == 'A') { // code for arrow up
				next = dlist_prev(last);
			} else if (cmd.buf[0] == 'B') { // code for arrow down
				next = dlist_next(last);
			}
			if (last && last->data) {
				strcpy(cmd.buf, last->data);
				start = strlen(last->data);
			} else {
				cmd.buf[0] = 0;
				start = 0;
			}
			last = next;
			continue;
		}
		if (!strcmp(cmd.buf, "quit")) {
			exit(0);
		} else {
			dlist_append(his_cmds, __strdup(cmd.buf));
			last = dlist_tail(his_cmds);
			execute_cmd(client, cmd.buf, -1);
			start = 0;
			cmd.buf[0] = 0;
		}
	}
}

void get_prompt(struct sftt_client_v2 *client, char *prompt, int len)
{
	char sub_path[DIR_PATH_MAX_LEN];
	char *pos;

	pos = basename(client->pwd);
	if (pos == NULL)
		strcpy(sub_path, "~");
	else {
		strncpy(sub_path, pos, DIR_PATH_MAX_LEN);
	}

	snprintf(prompt, len, "[%s@%s %s]$ ", client->uinfo.name,
			client->host, sub_path);
}

int reader_loop2(struct sftt_client_v2 *client)
{
	char cmd[CMD_MAX_LEN];
	char prompt[PROMPT_MAX_LEN];
	char *line;

	his_cmds = dlist_create(FREE_MODE_MP_FREE);

	for (;;) {
		get_prompt(client, prompt, PROMPT_MAX_LEN - 1);
#if 0
		printf("%s", prompt);
		fgets(cmd, CMD_MAX_LEN - 1, stdin);
#endif
		line = readline(prompt);
		if (line == NULL) {
			exit(0);
		}

		if (strlen(line) == 0)
			continue;

		/*
		 * no '\n' at the end of the line
		 * line[strlen(line) - 1] = 0;
		 */

		if (!strcmp(line, "quit")) {
			exit(0);
		}

		execute_cmd(client, line, -1);

		add_history(line);
#if 0
		dlist_append(his_cmds, __strdup(cmd));
#endif
	}
}

int try_fetch_login_info(char *input, char *user_name, char *host)
{
	int len = 0;
	char *p = NULL;
	if (input == NULL || user_name == NULL || host == NULL)
	       return -1;

	strip(input);
	len = strlen(input);
	if (!len)
		return -1;

	if ((p = strchr(input, '@')) && (strchr(p + 1, '@') == NULL)) {
		strncpy(user_name, input, p - input);
		strcpy(host, p + 1);

		return 0;
	}

	return -1;
}

bool is_remote_path(char *buf)
{
	char *pos;

	if ((pos = index(buf, '@')) == NULL)
		return false;

	if ((pos = index(pos, ':')) == NULL)
		return false;

	return true;
}

int fetch_remote_info(char *buf, char *user_name, char *host, char *path)
{
	char *pos;

	pos = index(buf, '@');
	if (pos == NULL || buf == pos)
	       return -1;

	strncpy(user_name, buf, pos - buf);

	buf = pos + 1;
	pos = index(buf, ':');
	if (pos == NULL || buf == pos)
		return -1;

	strncpy(host, buf, pos - buf);

	buf = pos + 1;
	strncpy(path, buf, FILE_NAME_MAX_LEN);

	if (strlen(path) == 0)
		return -1;

	return 0;
}

int try_fetch_trans_info(char *arg1, char *arg2, char *user_name,
	char *host, struct trans_info *trans)
{
	int ret;

	if (is_remote_path(arg1)) {
		trans->type = TRANS_GET;

		/* copy src */
		ret = fetch_remote_info(arg1, user_name, host, trans->src);
		if (ret)
			return ret;

		/* copy dest */
		strncpy(trans->dest, arg2, FILE_NAME_MAX_LEN);

	} else {
		if (!file_existed(arg1))
			return -1;

		if (!is_remote_path(arg2))
			return -1;

		trans->type = TRANS_PUT;

		/* copy src */
		strncpy(trans->src, arg1, FILE_NAME_MAX_LEN);

		/* copy dest */
		ret = fetch_remote_info(arg2, user_name, host, trans->dest);
		if (ret)
			return ret;
	}

	return 0;
}

int do_trans(struct sftt_client_v2 *client, struct trans_info *trans)
{
	char *args[2];
	bool argv_check = true;
	int ret;

	if (trans->type == TRANS_GET) {
		args[0] = trans->src;
		args[1] = trans->dest;

		return sftt_client_get_handler(client, 2, args, &argv_check);
	} else {
		args[0] = trans->dest;

		ret = sftt_client_cd_handler(client, 1, args, &argv_check);
		if (ret)
			return ret;

		args[0] = trans->src;

		return sftt_client_put_handler(client, 1, args, &argv_check);
	}
}

int execute_multi_cmds(struct sftt_client_v2 *client, char *buf)
{
	char cmd[CMD_MAX_LEN];
	char *p;

	memset(cmd, 0, sizeof(cmd));

	printf("begin to execute multi commands ...\n");
	p = strtok(buf, "\n");
	while (p) {
		strncpy(cmd, p, sizeof(cmd) - 1);
		printf("execute command: %s\n", cmd);
		execute_cmd(client, cmd, -1);
		p = strtok(NULL, "\n");
	}
	printf("execute multi commands done!\n");

	return 0;
}

int do_builtin(struct sftt_client_v2 *client, char *builtin)
{
	int i = 0;
	char buf[10 * CMD_MAX_LEN];

	memset(buf, 0, sizeof(buf));
	for (i = 0; builtin_scripts[i].name; ++i) {
		if (strcmp(builtin, builtin_scripts[i].name) == 0) {
			strncpy(buf, builtin_scripts[i].script, sizeof(buf) - 1);
			return execute_multi_cmds(client, buf);
		}
	}

	printf("unknown builtin script name: %s\n", builtin);

	return -1;
}

int sftt_client_who_handler(void *obj, int argc, char *argv[],
		bool *argv_check)
{
	struct sftt_client_v2 *client = obj;
	struct friend_user *p;
	int i = 0;
	char *hint;

	clear_friend_list(client);
	if (get_friend_list(client) == -1) {
		printf("get user list failed!\n");
		return -1;
	}

	list_for_each_entry(p, &client->friends, list) {
		if (strcmp(client->session_id, p->info.session_id) == 0)
			hint = " (me)";
		else
			hint = "";
		printf("%d\t%s\t%s\t%d\t%s%s\n", i++, p->info.name,
				p->info.ip, p->info.port,
				p->info.session_id, hint);
	}

	return 0;
}

void sftt_client_who_usage(void)
{
	printf("Usage: w\n");
}
