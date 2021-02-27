#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h> 
#if __linux__
#endif
#include <netinet/in.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <curses.h>
#include <ctype.h>
#include "random_port.h"
#include "command.h"
#include "config.h"
#include "client.h"
#include "encrypt.h"
#include "user.h"
#include "net_trans.h"
#include "validate.h"
#include "cmdline.h"
#include "packet.h"
#include "debug.h"
#include "state.h"
#include "version.h"

extern int errno;

path_entry *get_file_path_entry(char *file_name) {
	mem_pool *mp = get_singleton_mp();

	path_entry *pe = (path_entry *)mp_malloc(mp, sizeof(path_entry));
	if (pe == NULL) {
		return NULL;
	}	
	realpath(file_name, pe->abs_path);
	char *p = basename(pe->abs_path);
	strcpy(pe->rel_path, p);
	
	return pe;
}

void free_path_entry_list(path_entry_list *head) {
	path_entry_list *p = head, *q = head;
	while (p) {
		q = p->next;
		free(p);
		p = q;
	}
}
int is_dir(char *file_name) {
	struct stat file_stat;
	stat(file_name, &file_stat);	

	return S_ISDIR(file_stat.st_mode);
}

int is_file(char *file_name) {
	struct stat file_stat;
	stat(file_name, &file_stat);	

	return S_ISREG(file_stat.st_mode);
}

int file_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size) {
	int ret = fread(buffer, 1, size, fis->fp);		
	
	return ret;
}

int dir_get_next_buffer(struct file_input_stream *fis, char *buffer, size_t size) {
	return 0;
}

int new_connect(char *ip, int port, sftt_client_config *config, sock_connect *psc) {
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;  //使用IPv4地址
    	serv_addr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
	serv_addr.sin_port = htons(port);  //端口
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

sftt_client *create_client(char *ip, sftt_client_config *config, int connects_num) {
	mem_pool *mp = get_singleton_mp();

	sftt_client *client = (sftt_client *)mp_malloc(mp, sizeof(sftt_client));
	if (client == NULL) {
		return NULL;
	}
	memset(client, 0, sizeof(sftt_client));

	strcpy(client->ip, ip);
	printf("server ip is: %s\n", ip);

	int port = get_cache_port();
	printf("cache port is %d\n", port);
	
	int i = 0, ret = 0;
	sock_connect sc;
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

file_input_stream *create_file_input_stream(char *file_name) {
	mem_pool *mp = get_singleton_mp();

	if (strlen(file_name) > FILE_NAME_MAX_LEN) {
		return NULL;
	}
	file_input_stream *fis = (file_input_stream *)mp_malloc(mp, sizeof(file_input_stream));
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

void destory_file_input_stream(file_input_stream *fis) {
	if (fis) {
		free(fis);
	}
}

int consult_block_size_with_server(int sock, sftt_client_config *client_config) {
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

int send_single_file(int sock, sftt_packet *sp, path_entry *pe) {
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

path_entry *get_dir_path_entry_array(char *file_name, char *prefix, int *pcnt) {
	mem_pool *mp = get_singleton_mp();

	*pcnt = 0;
	path_entry_list *head = get_dir_path_entry_list(file_name, prefix);			
	if (head == NULL) {
		return NULL;
	}

	int count = 0;
	path_entry_list *p = head;
	while (p) {
		++count;
		p = p->next;
	}

	path_entry *array = (path_entry *)mp_malloc(mp, sizeof(path_entry) * count);
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

path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix) {
	mem_pool *mp = get_singleton_mp();

	path_entry_list *head = NULL;
	path_entry_list *current_entry = NULL;
	path_entry_list *sub_list = NULL;

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
			path_entry_list *node = (path_entry_list *)mp_malloc(mp, sizeof(path_entry_list));
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

void destory_sftt_client(sftt_client *client) {
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
	thread_input_params *tip = (thread_input_params *)args;
	sftt_packet *sp = malloc_sftt_packet((tip->connect).block_size);
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

int send_complete_end_packet(int sock, sftt_packet *sp) {
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

int send_multiple_file(sftt_client *client, path_entry *pes, int count) {
	if (count == 0) {
		return -1;
	}
	
	int i = 0, ret = 0;
	pthread_t thread_ids[CLIENT_MAX_CONNECT_NUM];
	thread_input_params tips[CLIENT_MAX_CONNECT_NUM];
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

int find_unfinished_session(path_entry *pe, char *ip) {
	return 0;		
}

int file_trans_session_diff(file_trans_session *old_session, file_trans_session *new_seesion) {
	return 0;
}

int dir_trans_session_diff(dir_trans_session *old_session, dir_trans_session *new_session) {
	return 0;
}

int save_trans_session(sftt_client *client) {
	return 0;
}


int main2(int argc, char **argv) {
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
	sftt_client_config client_config;
	int ret = get_sftt_client_config(&client_config);
	if (ret == -1) {
		printf("Error. get client config failed!\n");
		return -1;
	}
	printf("reading config done!\nconfigured block size is: %d\n", client_config.block_size);

	int connects_num = 0; 
	sftt_client *client = NULL;
	if (is_file(target)) {
		connects_num = 1;
		client = create_client(ip, &client_config, connects_num);
		if (client == NULL) {
			printf("Error. create client failed!\n");
			return -1;
		} else {
			printf("create client successfully!\n");
		}

		path_entry *pe = get_file_path_entry(target);	
		if (pe == NULL) {
			printf("Error. get file path entry failed!\n");
			return -1;
		}

		sftt_packet *sp = malloc_sftt_packet(client->connects[0].block_size);
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
		path_entry *pes = get_dir_path_entry_array(target, prefix, &count);
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
	dlist_node *node = NULL;
	dlist *args_list = dlist_create(free);
	char arg[CMD_ARG_MAX_LEN];
	int arg_len = 0;
	char **argv = NULL;
	cmd_args_state state = INIT;
	mem_pool *mp = get_singleton_mp();

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
		argv = (char **)mp_malloc(mp, sizeof(char *) * (*argc));
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
	mem_pool *mp = get_singleton_mp();

	if (check_command_format(buf) == -1) {
		return NULL;
	}

	cmd = (struct user_cmd *)mp_malloc(mp, sizeof(struct user_cmd));
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

static int run_command(const struct command *cmd, int argc, char *argv[])
{
    printf("running command: %s\n", cmd->name);
    return cmd->fn(argc, argv);
}

void execute_cmd(char *buf, int flag) {
	//printf("%s\n", buf);
	int i = 0;
	struct user_cmd *cmd = parse_command(buf);
	if (!cmd) {
		printf("your input cannot be recognized!\n");
		return ;
	}
	printf("exec name: %s, argc: %d", cmd->name, cmd->argc);
	for (i = 0; i < cmd->argc; ++i) {
		printf(", %s", cmd->argv[i]);
	}
	printf("\n");

	for (i = 0; sftt_client_cmds[i].name != NULL; ++i) {
		if (!strcmp(cmd->name, sftt_client_cmds[i].name)) {
			run_command(&sftt_client_cmds[i], cmd->argc, cmd->argv);
		}
	}

	//DEBUG((DEBUG_INFO, "%s\n", buf));
}

bool user_name_parse(char *optarg, char *user_name, int max_len) {
	int len = strlen(optarg);
	if (!(0 < len && len <= max_len )) {
		return false;
	}	
	strcpy(user_name, optarg);

	return true;
}

bool host_parse(char *optarg, char *host, int max_len) {
	int len = strlen(optarg);
	if (!(0 < len && len <= max_len)) {
		return false;
	}
	strcpy(host, optarg);

	return true;
}

bool port_parse(char *optarg, int *port) {
	if (!isdigit(optarg)) {
		return false;
	}

	*port = atoi(optarg);

	return true;
}

static void version(void)
{
    printf("version " VERSION ", Copyright (c) 2020-2021 zhou min, zhou qiang\n");
}

static void help(int exitcode)
{
    version();
	printf("usage:\t" PROC_NAME " [-u user] [-h host] [-p password] [-P port]\n"
       "\t" PROC_NAME " -u root -h localhost [-P port] -p\n");
    exit(exitcode);
}

static int init_sftt_client_ctrl_conn(sftt_client_v2 *client, int port) {
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

static int validate_user_info(sftt_client_v2 *client) {
	sftt_packet *req = malloc_sftt_packet(VALIDATE_PACKET_MIN_LEN);
	if (!req) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req->type = PACKET_TYPE_VALIDATE_REQ;

	validate_req v_req;
	char *tmp = strncpy(v_req.name, client->uinfo->name, USER_NAME_MAX_LEN - 1);
	v_req.name_len = strlen(tmp);

	tmp = strncpy(v_req.passwd_md5, client->uinfo->passwd_md5, MD5_LEN);
	v_req.passwd_len = strlen(tmp);

	// how to serialize and deserialize properly ???
	req->content = (char *)&v_req;
	req->data_len = sizeof(validate_req);
	req->block_size = VALIDATE_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->conn_ctrl.sock, req);
	if (ret == -1) {
		return -1;
	}

	sftt_packet *resp = malloc_sftt_packet(VALIDATE_PACKET_MIN_LEN);
	if (!resp) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp);
	printf("%d, ret: %d\n", __LINE__, ret);
	if (ret == -1) {
		return -1;
	}

	validate_resp *v_resp = (validate_resp *)resp->content;
	if (v_resp->status != UVS_PASS) {
		return -1;
	}

	client->uinfo->uid = v_resp->uid;

	return 0;
}

static int init_sftt_client_session(sftt_client_v2 *client)
{

}

static int init_sftt_client_v2(sftt_client_v2 *client, char *host, int port, char *user, char *passwd) {
	strncpy(client->host, host, HOST_MAX_LEN - 1);

	client->mp = get_singleton_mp();
	client->uinfo = mp_malloc(client->mp, sizeof(user_info));
	strncpy(client->uinfo->name, user, USER_NAME_MAX_LEN - 1);
	strncpy(client->uinfo->passwd, passwd, USER_PASSWD_MAX_LEN - 1);
	if (strlen(passwd)) {
		md5_str(passwd, strlen(passwd), client->uinfo->passwd_md5);
		//printf("passwd_md5: %s\n", client->uinfo->passwd_md5);
		show_md5(client->uinfo->passwd_md5);
	} else {
		client->uinfo->passwd_md5[0] = 0;
	}

	if (init_sftt_client_ctrl_conn(client, port) == -1) {
		return -1;
	}

	if (init_sftt_client_session(client) == -1) {
		return -1;
	}

	return 0;
}

static int show_options(char *host, char *user_name, char *password) {
	printf("host: %s\n", host);
	printf("your name: %s\n", user_name);
	printf("your password: %s\n", password);
}

void sftt_client_ll_usage(void)
{
	printf("ll\n");
}

int sftt_client_ll_handler(int argc, char *argv[])
{
	printf("I'm %s handler of ll\n", __func__);
}

int sftt_client_help_handler(int argc, char *argv[])
{
	printf("sftt client commands:\n\n"
		"\tll	list contents of current directory.\n"
		"\tcd	change the current directory.\n"
		"\tpwd	get current directory.\n"
		"\tget	get the file on server.\n"
		"\tput	put the file to server.\n"
		"\this	get history command.\n"
		"\thelp	show help info.\n\n");
}

int sftt_client_his_handler(int argc, char *argv[])
{

}

void sftt_client_his_usage(void)
{
}

void sftt_client_help_usage(void)
{

}

int sftt_client_cd_handler(int argc, char *argv[])
{
	return 0;
}

void sftt_client_cd_usage(void)
{

}

int sftt_client_pwd_handler(int argc, char *argv[])
{

}

void sftt_client_pwd_usage(void)
{

}

int sftt_client_get_handler(int argc, char *argv[])
{

}

void sftt_client_get_usage(void)
{

}

int sftt_client_put_handler(int argc, char *argv[])
{

}

void sftt_client_put_usage(void)
{

}

/* Read and execute commands until user inputs 'quit' command */
int reader_loop(sftt_client_v2 *client)
{
	int start;
	cmd_line cmd;
	dlist_node *last = NULL, *next = NULL;
	dlist *his_cmds = dlist_create(free);

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
			execute_cmd(cmd.buf, -1);
			start = 0;
			cmd.buf[0] = 0;
		}
	}
}

int reader_loop2(sftt_client_v2 *client)
{
	char cmd[CMD_MAX_LEN];
	dlist *his_cmds = dlist_create(free);

	for (;;) {
		printf("sftt>> ");
		fgets(cmd, CMD_MAX_LEN - 1, stdin);
		cmd[strlen(cmd) - 1] = 0;
		if (!strcmp(cmd, "quit")) {
			exit(0);
		}
		execute_cmd(cmd, -1);
		dlist_append(his_cmds, strdup(cmd));
	}
}

int main(int argc, char **argv) {
	int optind = 1;
	char *optarg = NULL;
	bool has_passwd_opt = false;
	char user_name[USER_NAME_MAX_LEN];
	char password[USER_PASSWD_MAX_LEN];
	char host[HOST_MAX_LEN];
	int port = -1;
	const sftt_option *opt = NULL;	
	bool ret = false;
	int passwd_len = 0;

	memset(user_name, 0, sizeof(user_name));
	memset(password, 0, sizeof(password));
	memset(host, 0, sizeof(host));
	for (;;) {
		if (optind >= argc) {
			break;
		}
		opt = lookup_opt(argc, argv, &optarg, &optind, sftt_client_opts);
		if (opt == NULL) {
			printf("invalid option\n");
			help(-1);
		}
		switch (opt->index) {
		case USER:
			ret = user_name_parse(optarg, user_name, sizeof(user_name));
			if (!ret) {
				printf("user name is invalid!\n");	
				help(-1);
			}
			break;
		case HOST:
			ret = host_parse(optarg, host, sizeof(host));
			if (!ret) {
				printf("host is invalid!\n");
				help(-1);
			}
			break;
		case PORT:
			ret = port_parse(optarg, &port);
			if (!ret) {
				printf("port is invalid!\n");
				help(-1);
			}
			break;
		case PASSWORD:
			has_passwd_opt = true;
			break;
		}
	}

	if (has_passwd_opt) {
		passwd_len = get_pass("password: ", password, sizeof(password));
		if (passwd_len <= 0) {
			printf("password is invalid!\n");
			help(-1);
		}
	}

	if (strlen(user_name) == 0) {
		printf("user name is invalid!\n");
		help(-1);
	}

	if (strlen(host) == 0) {
		printf("host is invalid!\n");
		help(-1);
	}

#ifdef DEBUG_ENABLE
	show_options(host, user_name, password);
#endif

	sftt_client_v2 client;
	if (init_sftt_client_v2(&client, host, port, user_name, password) == -1) {
		printf("init sftt client failed!\n");
		exit(-1);
	}

	if (validate_user_info(&client) == -1) {
		printf("cannot validate user and password!\n");
		exit(-1);
	}

	printf("validate successfully!\n");

	reader_loop2(&client);

	return 0;
}
