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
#include "base.h"
#include "endpoint.h"
#include "command.h"
#include "config.h"
#include "context.h"
#include "client.h"
#include "encrypt.h"
#include "file.h"
#include "log.h"
#include "mkdirp.h"
#include "net_trans.h"
#include "validate.h"
#include "cmdline.h"
#include "packet.h"
#include "debug.h"
#include "req_rsp.h"
#include "state.h"
#include "user.h"
#include "utils.h"
#include "version.h"

extern int errno;
extern struct mem_pool *g_mp;

bool directcmd = false;

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

int new_connect(char *ip, int port, struct sftt_client_config *config,
	struct sock_connect *psc)
{
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(port);
	int ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		return -1;
	}

	printf("consulting block size with server ...\n");
	int consulted_block_size = consult_block_size_with_server(sock, config);
	if (consulted_block_size < 1) {
		printf("Error. consult block size with server failed!\n");
		return -1;
	}
	printf("consulting block size done!\nconsulted block size is: %d\n",
		consulted_block_size);

	psc->sock = sock;
	psc->block_size = consulted_block_size;

	return 0;
}

struct sftt_client *create_client(char *ip, struct sftt_client_config *config,
	int connects_num)
{
	struct sftt_client *client = (struct sftt_client *)mp_malloc(g_mp,
			__func__, sizeof(struct sftt_client));
	if (client == NULL) {
		return NULL;
	}
	memset(client, 0, sizeof(struct sftt_client));

	strcpy(client->ip, ip);
	printf("server ip is: %s\n", ip);

	int port = get_cache_port();
	printf("cache port is %d\n", port);
	
	int i = 0, ret = 0;
	struct sock_connect sc;
	for (i = 0; i < connects_num; ++i) {
		ret = new_connect(ip, port, config, &sc);
		if (ret == -1) {
			break;
		}	
		client->connects[client->connects_num++] = sc;
	}
	
	if (i > 0) {
		printf("sock connects num is: %d\n", i);
		client->port = port;
		return client;
	}
	
	port = get_random_port();
	printf("random port is %d\n", port);
	for (; i < connects_num; ++i) {
		ret = new_connect(ip, port, config, &sc);
		if (ret == -1) {
			break;
		}	
		client->connects[client->connects_num++] = sc;
	}

	if (i > 0) {
		printf("sock connects num is: %d\n", i);
		client->port = port;
		set_cache_port(port);
		return client;
	}

	fprintf(stderr, "Value of errno: %d\n", errno);
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	mp_free(g_mp, client);
	
	return NULL;
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

	memset(fis->target, 0, FILE_NAME_MAX_LEN + 1);
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
	char buffer[BUFFER_SIZE];
		
	memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
	sprintf(buffer, "%d", client_config->block_size);
	//printf("client block size is : %d\n", client_config.block_size);
	sftt_encrypt_func((char *)buffer, BUFFER_SIZE); 
	
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
	int consulted_block_size = atoi(buffer);
	//printf("consulted block size is: %d\n", consulted_block_size);
	
	return consulted_block_size; 
}

int send_single_file(int sock, struct sftt_packet *sp, struct path_entry *pe)
{
	FILE *fp = fopen(pe->abs_path, "rb");
	if (fp == NULL) {
		printf("Error. cannot open file: %s\n", pe->abs_path);
		return -1;
	}
	
	printf("sending file %s\n", pe->abs_path);

	sp->type = PACKET_TYPE_FILE_NAME_REQ;
	sp->data_len = strlen(pe->rel_path);
	strcpy(sp->content, pe->rel_path);
	int ret = send_sftt_packet(sock, sp);
	if (ret == -1) {
		printf("Error. send file name block failed!\n");
		return -1;
	}
	
	int read_count = 0, i = 0, j = 0;
	int prefix_len = PACKET_TYPE_SIZE;
	do {
		printf("%d-th transport ...\n", ++j);
		sp->type = PACKET_TYPE_DATA_REQ;
		read_count = fread(sp->content, 1, sp->block_size, fp);		
		printf("read block size: %d\n", read_count);
		if (read_count < 1) {
			break;
		}

		sp->data_len = read_count;
		ret = send_sftt_packet(sock, sp);
		if (ret == -1) {
			printf("Error. send data block failed!\n");
			return -1;
		}
	
	} while (read_count == sp->block_size);

	sp->type = PACKET_TYPE_FILE_END_REQ;
	sp->data_len = 0;
	ret = send_sftt_packet(sock, sp);
	if (ret == -1) {
		printf("Error. send file end block failed!\n");
		return -1;
	}

	printf("sending %s done!\n", pe->abs_path);


	return 0;
}

void destroy_sftt_client(struct sftt_client *client)
{
	if (client == NULL) {
		return ;
	}
	int i = 0;
	for (i = 0; i < client->connects_num; ++i) {
		close(client->connects[i].sock);
	}
}

void usage(char *exec_file)
{
	fprintf (stdout, "\nUsage: %s -h ip <input_file>\n\n", exec_file);
}

void *send_files_by_thread(void *args)
{
	struct thread_input_params *tip = (struct thread_input_params *)args;
	struct sftt_packet *sp = malloc_sftt_packet((tip->connect).block_size);
	if (sp == NULL) {
		printf("Error. malloc sftt packet failed!\n");
		return (void *)-1;
	}
	int i = 0, ret = 0;
	for (i = tip->index; i < tip->pe_count; i += tip->step) {
		ret = send_single_file((tip->connect).sock, sp, tip->pes + i);
		if (ret == -1) {
			printf("Error. send single file in thread %d failed!"
					"abs path: %s, rel path: %s\n",
					tip->index, tip->pes[i].abs_path,
					tip->pes[i].rel_path);
			break;
		}
	}

	send_complete_end_packet((tip->connect).sock, sp);

	return NULL;
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

int send_multiple_file(struct sftt_client *client, struct path_entry *pes,
	int count)
{
	if (count == 0) {
		return -1;
	}
	
	int i = 0, ret = 0;
	pthread_t thread_ids[CLIENT_MAX_CONNECT_NUM];
	struct thread_input_params tips[CLIENT_MAX_CONNECT_NUM];
	for (i = 0; i < client->connects_num; ++i) {
		tips[i].connect = client->connects[i];
		tips[i].index = i;
		tips[i].step = client->connects_num;
		tips[i].pes = pes;
		tips[i].pe_count = count;
		ret = pthread_create(thread_ids + i, NULL, send_files_by_thread, tips + i);
		if (ret != 0) {
			printf("Error. thread %d create failed!\n", i);
			return -1;
		}
	} 

	void *pret = NULL;
	for (i = 0; i < client->connects_num; ++i) {
		pthread_join(thread_ids[i], &pret);
		if(pret == (void*)-1)
		{
		    printf("Error. thread %d exit failed!\n", i);
		    ret = -1;
		}
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


int client_main_old(int argc, char **argv)
{
	if (argc < 4) {
		usage(argv[0]);
		return -1;
	}

	char *ip = NULL;
	char *target = NULL; 
	int i = 1;
	for (; i < argc; ++i) {
		if (strcmp(argv[i], "-h") == 0) {
			++i;
			if (i >= argc) {
				goto PARAMS_ERROR;
			}	
			ip = argv[i]; 
			continue;
		} 
		if (i == argc - 1) {
			target = argv[i];	
		}
	}

	if (ip == NULL || !is_valid_ipv4(ip)) {
		printf("Error. ip is invalid: %s\n", (ip == NULL) ? "NULL" : ip);
		return -1;
	}

	if (target == NULL) {
		goto PARAMS_ERROR;
	}
	if (strlen(target) > FILE_NAME_MAX_LEN) {
		printf("Error. File name too long: %s\n", target);
		return -1;
	} 

	printf("reading config ...\n");
	struct sftt_client_config client_config;
	int ret = get_sftt_client_config(&client_config);
	if (ret == -1) {
		printf("Error. get client config failed!\n");
		return -1;
	}
	printf("reading config done!\nconfigured block size is: %d\n",
		client_config.block_size);

	int connects_num = 0; 
	struct sftt_client *client = NULL;
	if (is_file(target)) {
		connects_num = 1;
		client = create_client(ip, &client_config, connects_num);
		if (client == NULL) {
			printf("Error. create client failed!\n");
			return -1;
		} else {
			printf("create client successfully!\n");
		}

		struct path_entry *pe = get_file_path_entry(target);	
		if (pe == NULL) {
			printf("Error. get file path entry failed!\n");
			return -1;
		}

		struct sftt_packet *sp = malloc_sftt_packet(client->connects[0].block_size);
		if (sp == NULL) {
			printf("Error. malloc sftt packet failed!\n");
			return -1;
		}

		send_single_file(client->connects[0].sock, sp, pe);
		send_complete_end_packet(client->connects[0].sock, sp);

		free_sftt_packet(&sp);
		mp_free(g_mp, pe);

	} else if (is_dir(target)) {
		connects_num = CLIENT_MAX_CONNECT_NUM;
		client = create_client(ip, &client_config, connects_num);
		if (client == NULL) {
			printf("Error. create client failed!\n");
			return -1;
		} else {
			printf("create client successfully!\n");
		}

		char prefix[1] = {0};
		int count = 0;
		struct path_entry *pes = get_dir_path_entry_array(target, prefix, &count);
		if (pes == NULL) {
			printf("Error. get dir path entry list failed!\n");
			return -1;
		}

		send_multiple_file(client, pes, count);

		mp_free(g_mp, pes);
	}

	destroy_sftt_client(client);

	return 0;

PARAMS_ERROR:
	usage(argv[0]);

	return -1;
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

void execute_directcmd(struct sftt_client_v2 *client, char *buf)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct directcmd_req *req_info;
	struct directcmd_resp *resp_info;
	int ret = 0;
	long total_len = 0;

	if (strlen(buf) == 0)
		return;

	req_packet = malloc_sftt_packet(DIRECTCMD_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return;
	}
	req_packet->type = PACKET_TYPE_DIRECTCMD_REQ;

	req_info = mp_malloc(g_mp, "directcmd_handler_req", sizeof(struct directcmd_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	strncpy(req_info->cmd, buf, CMD_MAX_LEN - 1);

	req_packet->obj = req_info;
	req_packet->block_size = DIRECTCMD_REQ_PACKET_MIN_LEN;

	ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return;
	}

	resp_packet = malloc_sftt_packet(DIRECTCMD_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return;
	}

	resp_info = (struct directcmd_resp *)resp_packet->obj;
	assert(resp_info != NULL);

	if (resp_info->total_len < 0 || resp_info->this_len < 0) {
		printf("command execute failed!\n");
		return;
	}

	while (total_len < resp_info->total_len) {
		printf("%s", resp_info->data);

		total_len += resp_info->this_len;
		if (total_len == resp_info->total_len)
			break;

		ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			return;
		}

		resp_info = (struct directcmd_resp *)resp_packet->obj;
		assert(resp_info != NULL);
	}

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);
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

void execute_cmd(struct sftt_client_v2 *client, char *buf, int flag)
{
	int i = 0;
	bool found = false;
	struct user_cmd *cmd = NULL;

	add_log(LOG_INFO, "input command: %s", buf);

	cmd = parse_command(buf);
	if (!cmd) {
		printf("sftt client: cannot find command: %s\n"
			"please input 'help' to get the usage.\n", buf);
		return ;
	}

	if (directcmd && among_directcmds(cmd->name))
		return execute_directcmd(client, buf);

	add_cmd_log(cmd);
	for (i = 0; sftt_client_cmds[i].name != NULL; ++i) {
		if (!strcmp(cmd->name, sftt_client_cmds[i].name)) {
			found = true;
			run_command(client, &sftt_client_cmds[i], cmd->argc, cmd->argv);
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
    version();
	printf("usage:\t" PROC_NAME " [-u user] [-h host] [-p password] [-P port]\n"
       "\t" PROC_NAME " -u root -h localhost [-P port] -p\n");
    exit(exitcode);
}

static int init_sftt_client_ctrl_conn(struct sftt_client_v2 *client, int port)
{
	assert(client);
	if (port == -1) {
		port = get_random_port();
	}

	client->conn_ctrl.sock = make_connect(client->host, port);
	if (client->conn_ctrl.sock == -1) {
		return -1;
	}

	client->conn_ctrl.type = CONN_TYPE_CTRL;
	client->conn_ctrl.port = port;

	return 0;
}

static int validate_user_base_info(struct sftt_client_v2 *client, char *passwd)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct validate_req *req_info;
	struct validate_resp *resp_info;

	req_packet = malloc_sftt_packet(VALIDATE_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_VALIDATE_REQ;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct validate_req));
	assert(req_info != NULL);

	strncpy(req_info->name, client->uinfo->name, USER_NAME_MAX_LEN - 1);
	req_info->name_len = strlen(req_info->name);

	if (strlen(passwd)) {
		md5_str(passwd, strlen(passwd), req_info->passwd_md5);
	} else {
		req_info->passwd_md5[0] = 0;
	}
	req_info->passwd_len = strlen(req_info->passwd_md5);
	req_info->task_port = client->task_handler.port;

	req_packet->obj = req_info;
	req_packet->block_size = VALIDATE_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet(VALIDATE_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (validate_resp *)resp_packet->obj;
	if (resp_info->status != UVS_PASS) {
		printf("%s: validate status is not pass! status: 0x%0x\n",
			__func__, resp_info->status);
		switch (resp_info->status) {
		case UVS_NTFD:
			printf("user %s not found!\n", req_info->name);
			break;
		case UVS_INVALID:
			printf("user name and passwd not match!\n");
			break;
		case UVS_MISSHOME:
			printf("user %s's home dir cannot access!\n",
				req_info->name);
			break;
		case UVS_BLOCK:
			printf("user %s blocked!\n", req_info->name);
			break;
		default:
			printf("validate exception!\n");
			break;
		}
		return -1;
	}

	client->uinfo->uid = resp_info->uid;
	add_log(LOG_INFO, "uid: %d", client->uinfo->uid);

	strncpy(client->session_id, resp_info->session_id, SESSION_ID_LEN - 1);
	strncpy(client->pwd, resp_info->pwd, DIR_PATH_MAX_LEN - 1);

	//mp_free(g_mp, req_info);

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return 0;
}

static int init_sftt_client_session(struct sftt_client_v2 *client)
{
	return 0;
}

struct client_task *create_client_task(struct client_task_handler *handler)
{
	struct client_task *task;

	task = mp_malloc(g_mp, "client_task", sizeof(struct client_task));
	if (task == NULL)
		return NULL;

	INIT_LIST_HEAD(&task->list);

	handler->pm->ops->lock(handler->pm);
	if (handler->tasks == NULL) {
		handler->tasks = task;
	} else {
		list_add(&task->list, &handler->tasks->list);
	}

	handler->pm->ops->unlock(handler->pm);

	return task;
}

void *do_task(void *arg)
{
	struct client_task *task;

	task = (struct client_task *)arg;

	while (1) {
	}

	return NULL;
}

void clean_task(struct client_task_handler *handler)
{

}



void *sftt_client_task_handler(void *arg)
{
	struct sftt_client_v2 *client;
	struct sockaddr_in addr_server;
	int conn_fd;
	int len;
	int ret;
	struct client_task *task;

	client = (struct sftt_client_v2 *)arg;
	len = sizeof(struct sockaddr_in);
	while (1) {
		conn_fd = accept(client->task_handler.sock, (struct sockaddr *)&addr_server,
			&len);
		if (conn_fd == -1) {
			usleep(100 * 1000);
			continue;
		}
		printf("server is connected, ip=%s, port=%d\n", inet_ntoa(addr_server.sin_addr),
				ntohs(addr_server.sin_port));
		task = create_client_task(&client->task_handler);
		if (task == NULL) {
			printf("cannot create client task!\n");
			close(conn_fd);
		}
		task->sock = conn_fd;

		ret = pthread_create(&task->tid, NULL, do_task, task);
		if (ret == -1) {
			perror("create task thread failed");
			task->status = TASK_STATUS_ABORT;
			close(task->sock);
		}

		clean_task(&client->task_handler);
	}
}

int init_sftt_client_task_handler(struct sftt_client_v2 *client)
{
	struct sockaddr_in taskaddr;
	int len;

	client->task_handler.sock = -1;
	client->task_handler.port = -1;

	if ((client->task_handler.sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("create socket failed");
		goto failed;
	}

	if (fcntl(client->task_handler.sock, F_SETFL, O_NONBLOCK) == -1) {
		perror("set sockfd to non-block failed");
		goto failed;
	}

	memset(&taskaddr, 0, sizeof(taskaddr));
	taskaddr.sin_family = AF_INET;
	taskaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	taskaddr.sin_port = 0;

	if (bind(client->task_handler.sock, (struct sockaddr *)&taskaddr, sizeof(taskaddr)) == -1) {
		perror("bind socket error");
		goto failed;
	}

	if (listen(client->task_handler.sock, 10) == -1) {
		perror("listen socket error");
		goto failed;
	}

	len = sizeof(struct sockaddr_in);
	if (getsockname(client->task_handler.sock, (struct sockaddr *)&taskaddr, &len) == -1) {
		perror("getsockname error");
		goto failed;
	}

	client->task_handler.port = ntohs(taskaddr.sin_port);
	client->task_handler.pm = new(pthread_mutex);
	if (client->task_handler.pm == NULL) {
		printf("create pthread_mutex for task_handler\n");
		goto failed;
	}

	client->task_handler.tasks = NULL;

	if (pthread_create(&client->task_handler.tid, NULL, sftt_client_task_handler, client) == -1) {
		perror("pthread create error");
		goto failed;
	}

	return 0;
failed:
	close(client->task_handler.sock);

	return -1;
}

int init_sftt_client_v2(struct sftt_client_v2 *client, char *host, int port,
	char *user, char *passwd)
{
	/*
	 * set client context globally
	 */
	set_current_context("client");
	set_log_type(CLIENT_LOG);

	strncpy(client->host, host, HOST_MAX_LEN - 1);

	client->mp = get_singleton_mp();
	client->uinfo = mp_malloc(client->mp, __func__, sizeof(struct user_base_info));
	strncpy(client->uinfo->name, user, USER_NAME_MAX_LEN - 1);

	if (get_sftt_client_config(&client->config) == -1) {
		printf("get sftt client config failed!\n");
		return -1;
	}

	logger_init(client->config.log_dir, PROC_NAME);

	if (init_sftt_client_task_handler(client) == -1) {
		printf("init sftt client task handler failed!\n");
		return -1;
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

	return 0;
}

int show_options(char *host, char *user_name, char *password)
{
	add_log(LOG_INFO, "host: %s", host);
	add_log(LOG_INFO, "your name: %s", user_name);
	add_log(LOG_INFO, "your password: %s", password);
}

void sftt_client_ll_usage(void)
{
	printf("Usage: ll [path]\n");
}

int sftt_client_ll_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct ll_req *req_info;
	struct ll_resp *resp_info;
	struct sftt_client_v2 *client = obj;
	struct dlist *fe_list;
	struct file_entry *entry;
	struct dlist_node *node;
	char *path = NULL;
	int ret, i = 0;

	if (argc > 1 || argc < 0) {
		sftt_client_ll_usage();
		return -1;
	}

	if (argc == 0)
		path = client->pwd;
	else
		path = argv[0];

	req_packet = malloc_sftt_packet(LL_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_LL_REQ;

	req_info = mp_malloc(g_mp, "ll_handler_req", sizeof(struct ll_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	strncpy(req_info->path, path, DIR_PATH_MAX_LEN - 1);

	req_packet->obj = req_info;
	req_packet->block_size = LL_REQ_PACKET_MIN_LEN;

	ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet(LL_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	fe_list = dlist_create(FREE_MODE_MP_FREE);
	assert(fe_list != NULL);

	resp_info = (struct ll_resp *)resp_packet->obj;
	assert(resp_info != NULL);

	while (resp_info->nr > 0) {
		for (i = 0; i < resp_info->nr; ++i) {
			entry = mp_malloc(g_mp, "ll_handler_resp_file_entry",
					sizeof(struct file_entry));
			assert(entry != NULL);
			*entry = resp_info->entries[i];
			dlist_append(fe_list, entry);
		}

		if (resp_info->idx == -1)
			break;

		ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			return -1;
		}

		resp_info = (struct ll_resp *)resp_packet->obj;
		assert(resp_info != NULL);
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
	int i;
	*argv_check = true;
	printf("sftt client commands:\n\n");
	for (i = 0; sftt_client_cmds[i].name != NULL; ++i) {
		printf("\t%s\t\t%s\n", sftt_client_cmds[i].name, sftt_client_cmds[i].help);
	}
	printf("\t%s\t\t%s\n", "quit", "quit this session");

	return 0;
}

int sftt_client_his_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	int i = 0, num = 10;
	if (argc > 0) {
		num = atoi(argv[0]);
		add_log(LOG_INFO, "his number specified: %d", num);
	}

	struct dlist_node *node;
	dlist_for_each(his_cmds, node) {
		if (i >= num) {
			break;
		}
		printf("%s\n", (char *)node->data);
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

	if (argc != 1 || strlen(argv[0]) == 0) {
		sftt_client_cd_usage();
		return -1;
	}

	req_info = mp_malloc(g_mp, __func__, sizeof(struct cd_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	strncpy(req_info->path, argv[0], DIR_PATH_MAX_LEN);

	req_packet = malloc_sftt_packet(CD_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_CD_REQ;

	req_packet->obj = req_info;
	req_packet->block_size = CD_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet(CD_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (struct cd_resp *)resp_packet->obj;
	if (resp_info->status != RESP_OK) {
		printf("change directory failed!\n");
		ret = -1;
	} else {
		printf("pwd change to: %s\n", resp_info->pwd);
		strncpy(client->pwd, resp_info->pwd, DIR_PATH_MAX_LEN - 1);
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

	req_packet = malloc_sftt_packet(PWD_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_PWD_REQ;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct pwd_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);

	req_packet->obj = req_info;
	req_packet->block_size = PWD_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet(PWD_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (pwd_resp *)resp_packet->obj;
	printf("pwd: %s\n", resp_info->pwd);

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return 0;
}

void sftt_client_pwd_usage(void)
{
	printf("Usage: pwd\n");
}

int recv_one_file_by_get_resp(struct sftt_client_v2 *client,
	struct sftt_packet *resp_packet, struct common_resp *com_resp,
	char *target, bool *has_more)
{
	int ret = 0;
	int total_size = 0;
	char file[FILE_NAME_MAX_LEN];
	char md5[MD5_STR_LEN];
	char *rp = NULL;
	struct get_resp *resp = NULL;
	FILE *fp = NULL;

	*has_more = true;

	/* recv file name */
	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	resp = resp_packet->obj;

	if (!(resp->nr > 0)) {
		printf("%s: target file not exist\n", __func__);
		return -1;
	}
	assert(resp->entry.idx == 0);

	strncpy(file, resp->entry.content, FILE_NAME_MAX_LEN);
	rp = path_join(target, file);

	printf("get one file: %s\n", rp);
	if (resp->entry.type == FILE_TYPE_DIR) {
		if (!file_existed(rp)) {
			ret = mkdirp(rp, resp->entry.mode);
			if (ret == -1) {
				printf("create dir failed: %s\n", rp);
				return -1;
			}
		}

		goto recv_one_file_done;
	}

	/* recv md5 */
	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	resp = resp_packet->obj;

	strncpy(md5, resp->entry.content, MD5_STR_LEN);
	if (same_file(rp, md5)) {
		printf("file not changed: %s\n", rp);

		/* send resp */
		com_resp->status = RESP_OK;
		resp_packet->obj = com_resp;
		resp_packet->type = PACKET_TYPE_COMMON_RSP;

		ret = send_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}

		goto recv_one_file_done;
	} else {
		/* send resp */
		com_resp->status = CONTINUE;
		resp_packet->obj = com_resp;
		resp_packet->type = PACKET_TYPE_COMMON_RSP;

		ret = send_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}
	}

	fp = fopen(rp, "w+");
	if (fp == NULL) {
		printf("%s: open file for write failed!\n", __func__);
		printf("file: %s\n", rp);
		return -1;
	}

	do {
		/* recv content */
		ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}
		resp = resp_packet->obj;
		//printf("receive block len: %d\n", resp->entry.len);

		fwrite(resp->entry.content, resp->entry.len, 1, fp);

		/* send resp */
		com_resp->status = RESP_OK;
		resp_packet->obj = com_resp;
		resp_packet->type = PACKET_TYPE_COMMON_RSP;

		ret = send_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			break;
		}

		total_size += resp->entry.len;

	} while (total_size < resp->entry.total_size);

	fclose(fp);

	if (total_size < resp->entry.total_size) {
		printf("%s: recv one file failed: %s\n", __func__, rp);
		return -1;
	}

	if (!same_file(rp, md5)) {
		printf("%s: recv one file failed: %s, "
			"md5 not correct!\n", __func__, rp);
		return -1;
	}

recv_one_file_done:
	if (resp->idx == resp->nr - 1)
		*has_more = false;

	set_file_mode(rp, resp->entry.mode);

	printf("%s:%d, recv %s done!\n", __func__, __LINE__, rp);

	return 0;
}

int sftt_client_get_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct sftt_packet *req_packet;
	struct sftt_packet *resp_packet;
	struct get_req *req;
	struct get_resp *resp;
	struct sftt_client_v2 *client = obj;
	struct common_resp *com_resp;
	int ret;
	bool has_more = true;
	char *target = NULL;

	if (argc != 2) {
		sftt_client_get_usage();
		return -1;
	}

	target = argv[1];

	req_packet = malloc_sftt_packet(GET_REQ_PACKET_MIN_LEN);
	if (req_packet == NULL) {
		printf("%s: malloc sftt paceket failed!\n");
		return -1;
	}

	resp_packet = malloc_sftt_packet(GET_RESP_PACKET_MIN_LEN);
	if (resp_packet == NULL) {
		printf("%s: malloc sftt paceket failed!\n");
		return -1;
	}

	req = mp_malloc(g_mp, "get_handler_req", sizeof(struct get_req));
	if (req == NULL) {
		printf("%s: malloc get req failed!\n");
		return -1;
	}

	com_resp = mp_malloc(g_mp, "get_handler_com_resp", sizeof(struct common_resp));
	if (com_resp == NULL) {
		printf("%s: malloc common resp failed!\n");
		return -1;
	}

	/* send get req */
	strncpy(req->session_id, client->session_id, SESSION_ID_LEN);
	strncpy(req->path, argv[0], FILE_NAME_MAX_LEN);
	req_packet->obj = req;
	req_packet->type = PACKET_TYPE_GET_REQ;

	ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	do {
		ret = recv_one_file_by_get_resp(client, resp_packet,
			com_resp, target, &has_more);
	} while (ret == 0 && has_more);

	mp_free(g_mp, req);
	mp_free(g_mp, com_resp);

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return ret;
}

void sftt_client_get_usage(void)
{
	printf("Usage: get file|dir [file|dir]\n");
}

int send_trans_entry_by_put_req(struct sftt_client_v2 *client,
	struct sftt_packet *req_packet, struct put_req *req)
{
	req_packet->type = PACKET_TYPE_PUT_REQ;

	req_packet->obj = req;
	req_packet->block_size = PUT_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	return 0;
}

int send_file_name_by_put_req(struct sftt_client_v2 *client,
	struct sftt_packet *req_packet, char *path, char *fname,
	struct put_req *req)
{
	if (is_dir(path))
		req->entry.type = FILE_TYPE_DIR;
	else
		req->entry.type = FILE_TYPE_FILE;

	req->entry.mode = file_mode(path);
	strncpy(req->entry.content, fname, FILE_NAME_MAX_LEN);
	req->entry.len = strlen(fname);

	return send_trans_entry_by_put_req(client, req_packet, req);
}

int send_file_md5_by_put_req(struct sftt_client_v2 *client,
	struct sftt_packet *req_packet, char *file,
	struct put_req *req)
{
	int ret;

	if (is_dir(file))
		return 0;

	req->entry.total_size = file_size(file);

	ret = md5_file(file, req->entry.content);
	if (ret == -1)
		return -1;

	req->entry.len = strlen(req->entry.content);

	return send_trans_entry_by_put_req(client, req_packet, req);
}

int send_file_content_by_put_req(struct sftt_client_v2 *client,
	struct sftt_packet *req_packet, struct put_req *req)
{
	return send_trans_entry_by_put_req(client, req_packet, req);
}

int send_one_file_by_put_req(struct sftt_client_v2 *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet,
	char *path, char *fname, int nr, int idx)
{
	struct put_req *req;
	struct put_resp *resp;
	struct common_resp *com_resp;
	int ret;
	int len;
	int i = 0;
	FILE *fp;

	req = (struct put_req *)mp_malloc(g_mp, "send_one_file_req", sizeof(struct put_req));
	if (req == NULL)
		return -1;

	strncpy(req->session_id, client->session_id, SESSION_ID_LEN);
	req->nr = nr;
	req->idx = idx;
	req->entry.idx = 0;

	com_resp = (struct common_resp *)mp_malloc(g_mp, "send_one_file_com_resp",
			sizeof(struct common_resp));
	assert(com_resp != NULL);

	if (is_dir(path))
		return send_file_name_by_put_req(client, req_packet, path, fname, req);

	ret = send_file_name_by_put_req(client, req_packet, path, fname, req);
	if (ret == -1)
		return -1;
	req->entry.idx += 1;

	ret = send_file_md5_by_put_req(client, req_packet, path, req);
	if (ret == -1)
		return -1;
	req->entry.idx += 1;

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	com_resp = resp_packet->obj;
	if (com_resp->status == RESP_OK) {
		printf("file not changed: %s, skip ...\n", path);
		return 0;
	}

	printf("open file: %s\n", path);
	fp = fopen(path, "r");
	if (fp == NULL) {
		printf("open file failed: %s\n", path);
		return -1;
	}

	while (!feof(fp)) {
		ret = fread(req->entry.content, 1, CONTENT_BLOCK_SIZE, fp);
		//printf("read block size: %d\n", ret);

		req->entry.len = ret;
		ret = send_file_content_by_put_req(client, req_packet, req);
		if (ret == -1) {
			break;
		}

		ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}

		resp = resp_packet->obj;
		if (resp->status != RESP_OK) {
			printf("recv response failed!\n");
			break;
		}

		req->entry.idx += 1;
	}

	if (!feof(fp))
		ret = -1;

	fclose(fp);

	mp_free(g_mp, req);
	mp_free(g_mp, com_resp);

	return ret;
}

int sftt_client_put_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
	struct sftt_client_v2 *client = obj;
	FILE *fp;
	int i = 0;
	struct dlist *file_list;
	struct dlist_node *node;
	int file_count;
	char file[FILE_NAME_MAX_LEN];
	struct path_entry *entry;
	struct sftt_packet *req_packet;
	struct sftt_packet *resp_packet;

	if (argc != 1) {
		sftt_client_put_usage();
		return -1;
	}

	req_packet = malloc_sftt_packet(PUT_REQ_PACKET_MIN_LEN);
	if (req_packet == NULL) {
		printf("%s:%d, alloc req packet failed!\n", __func__, __LINE__);
		return -1;
	}

	resp_packet = malloc_sftt_packet(PUT_RESP_PACKET_MIN_LEN);
	if (resp_packet == NULL) {
		printf("%s:%d, alloc resp packet failed!\n", __func__, __LINE__);
		return -1;
	}

	strncpy(file, argv[0], FILE_NAME_MAX_LEN - 1);
	if (!is_file(file) && !is_dir(file)) {
		printf("cannot access: %s\n", file);
		return -1;
	}

	if (is_file(file)) {
		entry = get_path_entry(file, NULL);
		send_one_file_by_put_req(client, req_packet, resp_packet,
				entry->abs_path, entry->rel_path, 1, 0);
	} else {
		file_list = get_path_entry_list(file, NULL);
		file_count = dlist_size(file_list);
		dlist_for_each(file_list, node) {
			entry = node->data;
			printf("begin to send %s\n", entry->abs_path);
			if (send_one_file_by_put_req(client, req_packet, resp_packet,
				entry->abs_path, entry->rel_path, file_count, i) == -1) {
				printf("send file failed: %s\n", node->data);
			}
			++i;
		}
	}

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return 0;
}

void sftt_client_put_usage(void)
{
	printf("Usage: put file|dir\n");
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
	printf("client\t%d\t\t%d\t\t%d\t\t%d\n\n", stat.total_size,
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

	req_packet = malloc_sftt_packet(MP_STAT_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_MP_STAT_REQ;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct mp_stat_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);

	req_packet->obj = req_info;
	req_packet->block_size = MP_STAT_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet(MP_STAT_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (struct mp_stat_resp *)resp_packet->obj;
	get_mp_stat(g_mp, &stat);

	printf("\ttotal_size\ttotal_nodes\tusing_nodes\tfree_nodes\n");
	printf("client\t%d\t\t%d\t\t%d\t\t%d\n", stat.total_size,
		stat.total_nodes, stat.using_nodes, stat.free_nodes);
	printf("server\t%d\t\t%d\t\t%d\t\t%d\n", resp_info->total_size,
		resp_info->total_nodes, resp_info->using_nodes,
		resp_info->free_nodes);

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

int sftt_client_write_handler(void *obj, int argc, char *argv[],
		bool *argv_check)
{
	struct sftt_packet *req_packet, *resp_packet;
	struct write_req *req_info;
	struct write_resp *resp_info;
	struct sftt_client_v2 *client = obj;

	if (argc != 2) {
		sftt_client_write_usage();
		return -1;
	}

	req_packet = malloc_sftt_packet(WRITE_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_WRITE_REQ;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct write_req));
	assert(req_info != NULL);

	req_info->user_no = atoi(argv[0]);
	strncpy(req_info->message, argv[1], WRITE_MSG_MAX_LEN - 1);
	req_info->len = strlen(req_info->message);

	req_packet->obj = req_info;
	req_packet->block_size = WRITE_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet(WRITE_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	resp_info = (struct write_resp *)resp_packet->obj;

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

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

	snprintf(prompt, len, "[%s@%s %s]$ ", client->uinfo->name,
			client->host, sub_path);
}

int reader_loop2(struct sftt_client_v2 *client)
{
	char cmd[CMD_MAX_LEN];
	char prompt[PROMPT_MAX_LEN];
	his_cmds = dlist_create(FREE_MODE_MP_FREE);

	for (;;) {
		get_prompt(client, prompt, PROMPT_MAX_LEN - 1);
		printf("%s", prompt);
		fgets(cmd, CMD_MAX_LEN - 1, stdin);
		if (strlen(cmd) <= 1)
			continue;
		cmd[strlen(cmd) - 1] = 0;
		if (!strcmp(cmd, "quit")) {
			exit(0);
		}
		execute_cmd(client, cmd, -1);
		dlist_append(his_cmds, __strdup(cmd));
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
	struct sftt_packet *req_packet, *resp_packet;
	struct who_req *req_info;
	struct who_resp *resp_info;
	struct sftt_client_v2 *client = obj;
	struct dlist *user_list;
	struct dlist_node *node;
	struct logged_in_user *user;
	int ret, i = 0, total = 0;

	req_packet = malloc_sftt_packet(WHO_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_WHO_REQ;

	req_info = mp_malloc(g_mp, "who_handler_req", sizeof(struct who_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);

	req_packet->obj = req_info;
	req_packet->block_size = WHO_REQ_PACKET_MIN_LEN;

	ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	resp_packet = malloc_sftt_packet(WHO_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	user_list = dlist_create(FREE_MODE_MP_FREE);
	assert(user_list != NULL);

	resp_info = (struct who_resp *)resp_packet->obj;
	assert(resp_info != NULL);

	while (resp_info->total > 0 && resp_info->num > 0) {
		for (i = 0; i < resp_info->num; ++i) {
			user = mp_malloc(g_mp, "who_handler_resp_logged_in_user",
					sizeof(struct logged_in_user));
			assert(user != NULL);
			*user = resp_info->users[i];
			dlist_append(user_list, user);
		}

		total += resp_info->num;
		if (total == resp_info->total)
			break;

		ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			return -1;
		}

		resp_info = (struct who_resp *)resp_packet->obj;
		assert(resp_info != NULL);
	}

	dlist_for_each(user_list, node) {
		user = (struct logged_in_user *)node->data;
		printf("%s\t%s\t%d\n", user->name, user->ip, user->port);
	}

	dlist_destroy(user_list);

	free_sftt_packet(&req_packet);
	free_sftt_packet(&resp_packet);

	return 0;
}

void sftt_client_who_usage(void)
{
	printf("Usage: w\n");
}
