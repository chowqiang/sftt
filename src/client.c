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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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
#include "endpoint.h"
#include "command.h"
#include "config.h"
#include "client.h"
#include "encrypt.h"
#include "file.h"
#include "log.h"
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

struct sftt_option sftt_client_opts[] = {
	{"-u", USER, HAS_ARG},
	{"-h", HOST, HAS_ARG},
	{"-P", PORT, HAS_ARG},
	{"-p", PASSWORD, NO_ARG},
	{NULL, -1, NO_ARG}
};

struct dlist *his_cmds;

int consult_block_size_with_server(int sock, struct sftt_client_config *client_config);

int send_complete_end_packet(int sock, struct sftt_packet *sp);

struct path_entry *get_file_path_entry(char *file_name) {
	struct path_entry *pe = (struct path_entry *)mp_malloc(g_mp, sizeof(struct path_entry));
	if (pe == NULL) {
		return NULL;
	}	
	realpath(file_name, pe->abs_path);
	char *p = basename(pe->abs_path);
	strcpy(pe->rel_path, p);
	
	return pe;
}

void free_path_entry_list(struct path_entry_list *head) {
	struct path_entry_list *p = head, *q = head;
	while (p) {
		q = p->next;
		free(p);
		p = q;
	}
}

int file_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size) {
	int ret = fread(buffer, 1, size, fis->fp);		
	
	return ret;
}

int dir_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size) {
	return 0;
}

int new_connect(char *ip, int port, struct sftt_client_config *config, struct sock_connect *psc) {
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
	printf("consulting block size done!\nconsulted block size is: %d\n", consulted_block_size);	

	psc->sock = sock;
	psc->block_size = consulted_block_size;

	return 0;
}

struct sftt_client *create_client(char *ip, struct sftt_client_config *config, int connects_num) {
	struct sftt_client *client = (struct sftt_client *)mp_malloc(g_mp, sizeof(struct sftt_client));
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
	free(client);
	
	return NULL;
}

int get_cache_port() {
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

void set_cache_port(int port) {
	FILE *fp = fopen(PORT_CACHE_FILE, "w+");
	if (fp == NULL || port > MAX_PORT_NUM) {
		return ;
	}	

	char str[8];
	//itoa(port, str); 
	sprintf(str, "%d", port);
	fputs(str, fp);
}

struct file_input_stream *create_file_input_stream(char *file_name) {
	if (strlen(file_name) > FILE_NAME_MAX_LEN) {
		return NULL;
	}

	struct file_input_stream *fis = (struct file_input_stream *)mp_malloc(g_mp, sizeof(struct file_input_stream));
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

void destory_file_input_stream(struct file_input_stream *fis) {
	if (fis) {
		free(fis);
	}
}

int consult_block_size_with_server(int sock, struct sftt_client_config *client_config) {
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

int send_single_file(int sock, struct sftt_packet *sp, struct path_entry *pe) {
	FILE *fp = fopen(pe->abs_path, "rb");
	if (fp == NULL) {
		printf("Error. cannot open file: %s\n", pe->abs_path);
		return -1;
	}
	
	printf("sending file %s\n", pe->abs_path);

//	strcpy(sp->type, PACKET_TYPE_FILE_NAME);
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
//		strcpy(sp->type, PACKET_TYPE_DATA);
		sp->type = PACKET_TYPE_DATA_REQ;
		read_count = fread(sp->content, 1, sp->block_size, fp);		
		printf("read block size: %d\n", read_count);
		if (read_count < 1) {
			break;
		}

		/*
		for (i = 0; i < read_count; ++i) {
			printf("%c", buffer[i]);
		}
		*/
		sp->data_len = read_count;
		ret = send_sftt_packet(sock, sp);
		if (ret == -1) {
			printf("Error. send data block failed!\n");
			return -1;
		}
	
	} while (read_count == sp->block_size);

//	strcpy(sp->type, PACKET_TYPE_FILE_END);
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

struct path_entry *get_dir_path_entry_array(char *file_name, char *prefix, int *pcnt) {
	*pcnt = 0;
	struct path_entry_list *head = get_dir_path_entry_list(file_name, prefix);			
	if (head == NULL) {
		return NULL;
	}

	int count = 0;
	struct path_entry_list *p = head;
	while (p) {
		++count;
		p = p->next;
	}

	struct path_entry *array = (struct path_entry *)mp_malloc(g_mp, sizeof(struct path_entry) * count);
	if (array == NULL) {
		free_path_entry_list(head);
		return NULL;
	}
	
	int i = 0;
	p = head;
	for (i = 0; i < count; ++i) {
		if (p == NULL) {
			printf("Error. Unexpected fatal when copy path entry!\n");
		}
		strcpy(array[i].abs_path, (p->entry).abs_path);
		strcpy(array[i].rel_path, (p->entry).rel_path);
		p = p->next;
	}
	
	*pcnt = count;

	free_path_entry_list(head);
	
	return array;
}

struct path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix) {
	struct path_entry_list *head = NULL;
	struct path_entry_list *current_entry = NULL;
	struct path_entry_list *sub_list = NULL;

	char dir_abs_path[FILE_NAME_MAX_LEN];
	char dir_rel_path[FILE_NAME_MAX_LEN];
	char tmp_path[FILE_NAME_MAX_LEN];

	realpath(file_name, dir_abs_path);
	char *p = basename(dir_abs_path);
	sprintf(dir_rel_path, "%s/%s", prefix, p);
		
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
 
	if ((dp = opendir(file_name)) == NULL) {
		printf("Error. cannot open dir: %s\n", file_name);
		return NULL;
	}
	
	chdir(file_name);
	while ((entry = readdir(dp)) != NULL) {
		sprintf(tmp_path, "%s/%s", dir_abs_path, entry->d_name);
		lstat(tmp_path, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}
			//printf("dir tmp path: %s\n", tmp_path);
			sub_list = get_dir_path_entry_list(tmp_path, dir_rel_path);	
			if (sub_list == NULL) {
				continue;	
			}

			if (current_entry == NULL) {
				current_entry = head = sub_list;
			} else {
				current_entry->next = sub_list;
			}

			while (current_entry->next) {
				current_entry = current_entry->next;
			}				
				
		} else {
			struct path_entry_list *node = (struct path_entry_list *)mp_malloc(g_mp, sizeof(struct path_entry_list));
			if (node == NULL) {
				continue;
			}

			sprintf(node->entry.abs_path, "%s/%s", dir_abs_path, entry->d_name);
			sprintf(node->entry.rel_path, "%s/%s", dir_rel_path, entry->d_name);
			//printf("file: %s\n", node->entry.abs_path);
			node->next = NULL;

			if (current_entry == NULL) {
				current_entry = head = node;
			} else {
				current_entry->next = node;
				current_entry = node;
			}
		}
	}
	closedir(dp);	

	return head;
}

void destory_sftt_client(struct sftt_client *client) {
	if (client == NULL) {
		return ;
	}
	int i = 0;
	for (i = 0; i < client->connects_num; ++i) {
		close(client->connects[i].sock);
	}
}

void usage(char *exec_file) {
	fprintf (stdout, "\nUsage: %s -h ip <input_file>\n\n", exec_file);
}

void *send_files_by_thread(void *args) {
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
			printf("Error. send single file in thread %d failed! abs path: %s, rel path: %s\n", tip->index, tip->pes[i].abs_path, tip->pes[i].rel_path);
			break;
		}
	}

	send_complete_end_packet((tip->connect).sock, sp);

	return NULL;
}

int send_complete_end_packet(int sock, struct sftt_packet *sp) {
//	strcpy(sp->type, PACKET_TYPE_SEND_COMPLETE);
	sp->type = PACKET_TYPE_SEND_COMPLETE_REQ;
	sp->data_len = 0;
	int ret = send_sftt_packet(sock, sp);
	if (ret == -1) {
		printf("Error. send complete end block failed!\n");
		return -1;
	}	

	return 0;
}

int send_multiple_file(struct sftt_client *client, struct path_entry *pes, int count) {
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

int find_unfinished_session(struct path_entry *pe, char *ip) {
	return 0;		
}

int file_trans_session_diff(struct file_trans_session *old_session, struct file_trans_session *new_seesion) {
	return 0;
}

int dir_trans_session_diff(struct dir_trans_session *old_session, struct dir_trans_session *new_session) {
	return 0;
}

int save_trans_session(struct sftt_client *client) {
	return 0;
}


int client_main_old(int argc, char **argv) {
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
	printf("reading config done!\nconfigured block size is: %d\n", client_config.block_size);

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
		free(pe);

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

		free(pes);
	}

	destory_sftt_client(client);

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

	return strdup(exec_name);
}

char **parse_args(char *buf, int *argc)
{
	int i = 0;
	struct dlist_node *node = NULL;
	struct dlist *args_list = dlist_create(free);
	char arg[CMD_ARG_MAX_LEN];
	int arg_len = 0;
	char **argv = NULL;
	enum cmd_args_state state = INIT;

	for (i = 0; buf[i]; ++i) {
		if (arg_len >= CMD_ARG_MAX_LEN) {
			goto out;
		}
		if (isspace(buf[i])) {
			switch (state) {
			case INIT:
				if (arg_len) {
					arg[arg_len] = 0;
					dlist_append(args_list, strdup(arg));
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
				printf("unexpected state: %d in %s:%d\n", state, __FILE__, __LINE__);
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
		dlist_append(args_list, strdup(arg));
	}

	*argc = dlist_size(args_list);
	if (*argc) {
		argv = (char **)mp_malloc(g_mp, sizeof(char *) * (*argc));
		node = dlist_head(args_list);
		for (i = 0; i < *argc; ++i) {
			argv[i] = node->data;
			node->data = NULL;
			node = dlist_next(node);
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

	cmd = (struct user_cmd *)mp_malloc(g_mp, sizeof(struct user_cmd));
	if (!cmd) {
		return NULL;
	}

	cmd->name = parse_exec_name(buf, &offset);
	cmd->argv = parse_args(buf + offset, &cmd->argc);

	if (cmd->argc == -1) {
		return NULL;
	}

	return cmd;
}

static int run_command(struct sftt_client_v2 *client, const struct cmd_handler *cmd, int argc, char *argv[])
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
	char *buf = malloc(sizeof(char) * 1024);

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
	free(buf);
}

void execute_cmd(struct sftt_client_v2 *client, char *buf, int flag) {
	add_log(LOG_INFO, "input command: %s", buf);
	int i = 0;
	bool found = false;
	struct user_cmd *cmd = NULL;

	cmd = parse_command(buf);
	if (!cmd) {
		printf("sftt client: cannot find command: %s\n"
			"please input 'help' to get the usage.\n", buf);
		return ;
	}

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
}

bool parse_user_name(char *optarg, char *user_name, int max_len) {
	int len = strlen(optarg);
	if (!(0 < len && len <= max_len )) {
		return false;
	}	
	strcpy(user_name, optarg);

	return true;
}

bool parse_host(char *optarg, char *host, int max_len) {
	int len = strlen(optarg);
	if (!(0 < len && len <= max_len)) {
		return false;
	}
	strcpy(host, optarg);

	return true;
}

bool parse_port(char *optarg, int *port) {
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

static int init_sftt_client_ctrl_conn(struct sftt_client_v2 *client, int port) {
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

static int validate_user_base_info(struct sftt_client_v2 *client, char *passwd) {
	struct sftt_packet *req_packet, *resp_packet;
	struct validate_req *req_info;
	struct validate_resp *resp_info;

	req_packet = malloc_sftt_packet(VALIDATE_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_VALIDATE_REQ;

	req_info = mp_malloc(g_mp, sizeof(struct validate_req));
	assert(req_info != NULL);

	strncpy(req_info->name, client->uinfo->name, USER_NAME_MAX_LEN - 1);
	req_info->name_len = strlen(req_info->name);

	if (strlen(passwd)) {
		md5_str(passwd, strlen(passwd), req_info->passwd_md5);
		//printf("passwd_md5: %s\n", client->uinfo->passwd_md5);
		//char *md5_str = md5_printable_str(req_info->passwd_md5);
		//if (md5_str) {
		//	add_log(LOG_INFO, "%s", md5_str);
		//	free(md5_str);
		//}
	} else {
		req_info->passwd_md5[0] = 0;
	}
	req_info->passwd_len = strlen(req_info->passwd_md5);

	// how to serialize and deserialize properly ???
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
		//printf("%s: uid: 0x%0x, name: %s, session_id: %s\n",
		//	__func__, resp_info->uid, resp_info->name,
		//	resp_info->session_id);
		return -1;
	}

	client->uinfo->uid = resp_info->uid;
	add_log(LOG_INFO, "uid: %d", client->uinfo->uid);

	strncpy(client->session_id, resp_info->session_id, SESSION_ID_LEN - 1);
	strncpy(client->pwd, resp_info->pwd, DIR_PATH_MAX_LEN - 1);

	return 0;
}

static int init_sftt_client_session(struct sftt_client_v2 *client)
{

	return 0;
}

int init_sftt_client_v2(struct sftt_client_v2 *client, char *host, int port, char *user, char *passwd) {
	strncpy(client->host, host, HOST_MAX_LEN - 1);

	client->mp = get_singleton_mp();
	client->uinfo = mp_malloc(client->mp, sizeof(struct user_base_info));
	strncpy(client->uinfo->name, user, USER_NAME_MAX_LEN - 1);
	if (get_sftt_client_config(&client->config) == -1) {
		printf("get sftt client config failed!\n");
		return -1;
	}
	logger_init(client->config.log_dir, PROC_NAME);
	set_log_type(CLIENT_LOG);

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
		exit(-1);
	}

	return 0;
}

int show_options(char *host, char *user_name, char *password) {
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
	int i = 0;

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

	req_info = mp_malloc(g_mp, sizeof(struct ll_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	strncpy(req_info->path, path, DIR_PATH_MAX_LEN - 1);

	// how to serialize and deserialize properly ???
	req_packet->obj = req_info;
	req_packet->block_size = LL_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->conn_ctrl.sock, req_packet);
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

	fe_list = dlist_create(free);
	assert(fe_list != NULL);

	resp_info = (struct ll_resp *)resp_packet->obj;
	assert(resp_info != NULL);
	/*
	 * It's a bug from xdr lib, -1 was changed to 65535.
	 * I will fix this bug soon ...
	 */
	while (resp_info->idx != 65535) {
		//assert(resp_info->nr == FILE_ENTRY_MAX_CNT);
		for (i = 0; i < resp_info->nr; ++i) {
			entry = mp_malloc(g_mp, sizeof(struct file_entry));
			assert(entry != NULL);
			*entry = resp_info->entries[i];
			dlist_append(fe_list, entry);
		}

		ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			return -1;
		}

		resp_info = (struct ll_resp *)resp_packet->obj;
		assert(resp_info != NULL);
	}

	for (i = 0; i < resp_info->nr; ++i) {
		entry = mp_malloc(g_mp, sizeof(struct file_entry));
		assert(entry != NULL);
		*entry = resp_info->entries[i];
		dlist_append(fe_list, entry);
	}

	dlist_for_each(fe_list, node) {
		entry = (struct file_entry *)node->data;
		printf("%s\t%s\n", entry->type == 1 ? "file" : "dir", entry->name);
	}

	return 0;
}

int sftt_client_help_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
#if 0
	printf("sftt client commands:\n\n"
		"\tll	list contents of current directory.\n"
		"\tcd	change the current directory.\n"
		"\tpwd	get current directory.\n"
		"\tget	get the file on server.\n"
		"\tput	put the file to server.\n"
		"\this	get history command.\n"
		"\thelp	show help info.\n\n");
#endif
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
	req_packet = malloc_sftt_packet(CD_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_CD_REQ;

	req_info = mp_malloc(g_mp, sizeof(struct cd_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);
	strncpy(req_info->path, argv[0], DIR_PATH_MAX_LEN);

	// how to serialize and deserialize properly ???
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
	} else {
		printf("pwd change to: %s\n", resp_info->pwd);
		strncpy(client->pwd, resp_info->pwd, DIR_PATH_MAX_LEN - 1);
	}

	return 0;
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

	req_info = mp_malloc(g_mp, sizeof(struct pwd_req));
	assert(req_info != NULL);

	strncpy(req_info->session_id, client->session_id, SESSION_ID_LEN - 1);

	// how to serialize and deserialize properly ???
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


	return 0;

}

void sftt_client_pwd_usage(void)
{
	printf("Usage: pwd\n");
}

int sftt_client_get_handler(void *obj, int argc, char *argv[], bool *argv_check)
{

}

void sftt_client_get_usage(void)
{
	printf("Usage: get file|dir [file|dir]\n");
}

int sftt_client_put_handler(void *obj, int argc, char *argv[], bool *argv_check)
{
}

void sftt_client_put_usage(void)
{
	printf("Usage: put file|dir [file|dir]\n");
}

/* Read and execute commands until user inputs 'quit' command */
int reader_loop(struct sftt_client_v2 *client)
{
	int start;
	struct cmd_line cmd;
	struct dlist_node *last = NULL, *next = NULL;
	his_cmds = dlist_create(free);

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
			dlist_append(his_cmds, strdup(cmd.buf));
			last = dlist_tail(his_cmds);
			execute_cmd(client, cmd.buf, -1);
			start = 0;
			cmd.buf[0] = 0;
		}
	}
}

struct user_cmd *user_cmd_construct(void)
{
		
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
	his_cmds = dlist_create(free);

	for (;;) {
		get_prompt(client, prompt, PROMPT_MAX_LEN - 1);
		printf("%s", prompt);
		fgets(cmd, CMD_MAX_LEN - 1, stdin);
		cmd[strlen(cmd) - 1] = 0;
		if (!strcmp(cmd, "quit")) {
			exit(0);
		}
		execute_cmd(client, cmd, -1);
		dlist_append(his_cmds, strdup(cmd));
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
