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

#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>  
#include <signal.h>
#include <libgen.h>
#include <netinet/in.h>  
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/types.h> 
#include <sys/stat.h>  
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <time.h>
#include "autoconf.h"
#include "base.h"
#include "config.h"
#include "connect.h"
#include "context.h"
#include "debug.h"
#include "encrypt.h"
#include "file.h"
#include "lock.h"
#include "log.h"
#include "mem_pool.h"
#include "mkdirp.h"
#include "net_trans.h"
#include "endpoint.h"
#include "req_rsp.h"
#include "server.h"
#include "session.h"
#include "version.h"
#include "user.h"
#include "utils.h"

#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)  

struct sftt_option sftt_server_opts[] = {
	{"start", START, NO_ARG},
	{"restart", RESTART, NO_ARG},
	{"stop", STOP, NO_ARG},
	{"status", STATUS, NO_ARG},
	{"db", DB, NO_ARG},
	{"-d", DAEMON, NO_ARG},
	{"-s", STORE, HAS_ARG},
	{NULL, -1, NO_ARG}
};

extern struct mem_pool *g_mp;

struct sftt_server *server;

struct logger_init_ctx logger_ctx;

int create_non_block_sock(int *pport);

void put_sftt_server_stat(struct sftt_server_stat *sss);

struct sftt_server_stat *alloc_sftt_server_stat(void);

void sync_server_stat(void);

void server_init_func(struct sftt_server_config *server_config)
{
	DIR *mydir = NULL;
	if (get_sftt_server_config(server_config) != 0) {
		printf(PROC_NAME ": get server config failed!\n");
		exit(0);
	}
	char *filepath = server_config->store_path;
	add_log(LOG_INFO, "conf  block_size is %d", server_config->block_size);
	add_log(LOG_INFO, "store path: %s",filepath);
	if((mydir= opendir(filepath))==NULL) {
		int ret = mkdir(filepath, MODE);
		if (ret != 0) {
			printf(PROC_NAME ": create work dir failed!\n");
			exit(0);	
		}
	 }

}

int server_consult_block_size(int connect_fd,char *buff,int server_block_size)
{
	int trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
	if (trans_len <= 0 ) {
		printf("consult block size recv failed!\n");
		exit(0);
	}

	char *mybuff = sftt_decrypt_func(buff,trans_len);
	int client_block_size = atoi(buff);
	int min_block_size = client_block_size < server_block_size ?
		client_block_size : server_block_size;

	sprintf(buff,"%d",min_block_size);
	int size = strlen(buff);
	sftt_encrypt_func(buff,size);
	send(connect_fd,buff,BUFFER_SIZE,0);

	return min_block_size;

}

void server_file_resv(int connect_fd, int consulted_block_size,
	struct sftt_server_config init_conf)
{
	int trans_len;
	struct sftt_packet *sp = malloc_sftt_packet(consulted_block_size);
	int connected = 1;

	while (connected){
		FILE * fd;
		int i = 0 ;
		int j = 0 ; 
		char *data_buff = (char *)mp_malloc(g_mp,
			__func__, consulted_block_size * sizeof(char));

		memset(data_buff, '\0', consulted_block_size);
		while(1) {
			if (j >= 5) {
				printf(PROC_NAME " server idle times more than 5.\n");
				exit(0);
			}
			if (recv_sftt_packet(connect_fd, sp) != 0) {
				printf(PROC_NAME " server recv failed!\n");
				j ++ ;
				continue;
			}
			if (trans_len <= 0) {
				fclose(fd);
				printf("recev failed!\n");
				connected = 0;
				break;
			}
			switch (sp->type) {
			case PACKET_TYPE_FILE_NAME_REQ:
				fd = server_creat_file(sp,init_conf,data_buff);
				memset(data_buff,'\0',consulted_block_size);
				printf("get file name packet\n");
				i++;
				break;
			case PACKET_TYPE_DATA_REQ:
				server_transport_data_to_file(fd,sp);
				printf("get file type packet\n");
				i++;
				break;
			case PACKET_TYPE_FILE_END_REQ:
				printf("get file end packet\n");
				server_transport_data_to_file(fd,sp);
				i++;
				fclose(fd);
				break;
			case PACKET_TYPE_SEND_COMPLETE_REQ:
				printf("get complete packet\n");
				i++;
				break;
			default:
				printf("get unknown packet!\n ");
				i++;
				break;
			}
		}
	}
}

void server_transport_data_to_file(FILE *fd, struct sftt_packet *sp)
{
	int write_len=fwrite(sp->content, 1, sp->data_len, fd);
	add_log(LOG_INFO, "write len is %d", write_len);
}


FILE *server_creat_file(struct sftt_packet *sp,
	struct sftt_server_config init_conf, char *data_buff)
{
	int i;
	FILE * fd;
	data_buff = strcat(data_buff,init_conf.store_path);
	strcat(data_buff,sp->content);
			
	is_exit(data_buff);

	fd = fopen(data_buff,"w+");
	if (fd == NULL) {
		printf("create file received failed: %s\n ",data_buff);	
	}else{
		printf("create file received: %s\n",data_buff);
	}

	return fd;
}

void is_exit(char *filepath)
{
	char *tmp_path = (char *) malloc(strlen(filepath) * sizeof(char));
	printf("%s ======file_path\n",filepath);
	memset(tmp_path,'\0',strlen(filepath));
	strcpy(tmp_path,filepath);
	printf("tmp_path == %s ",tmp_path);
	int str_len = strlen(filepath);
	int i;

	for (i = 0; i <= str_len; i ++ ) {
		if (tmp_path[i] == '/'){ 
			tmp_path[i+1] = '\0';
			if (access(tmp_path, F_OK) == -1) {
				int status = mkdir(tmp_path,
					S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				if (status == -1) {
					printf("%s\n",tmp_path);
				}
			} else {
				printf("%s\n", tmp_path);
			}
			memset(tmp_path,'\0',str_len);
			strcpy(tmp_path,filepath);
			printf("%s=====",tmp_path);
			continue;
		}
	}
}

int server_main_old(void)
{
	int socket_fd = 0;
	int	connect_fd;
	int	trans_len;
	pid_t   pid;
	char	buff[BUFFER_SIZE] = {'\0'};
	char    quit[BUFFER_SIZE] = {'q','u','i','t'};
	struct sftt_server_config  init_conf;
	//init server 
	server_init_func(&init_conf);
	
	while(1){
		if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL,
			NULL)) == -1) {
			printf("connect filed");
			continue;
		}
		pid = fork();
		if (pid == 0) {
			int consulted_block_size;
			consulted_block_size = server_consult_block_size(
				connect_fd, buff, init_conf.block_size);
			printf("consulted_block_size : %d\n",consulted_block_size);
			server_file_resv(connect_fd, consulted_block_size, init_conf);
		} else if (pid < 0){
			printf("fork failed!\n");
		} else {
			wait(NULL);
		}
		
	}
	close(socket_fd);
}

void sighandler(int signum)
{
   add_log(LOG_INFO, "Caught signal %d, coming out ...", signum);
}

int init_sftt_server_stat(void)
{
	struct sftt_server_stat *sss = alloc_sftt_server_stat();
	assert(sss != NULL);

	sss->main_pid = getpid();
	memcpy(&sss->conf, &server->conf, sizeof(struct sftt_server_config));

	put_sftt_server_stat(sss);

	return 0;
}

int get_sftt_server_shmid(int create_flag)
{
	key_t key;
	int shmid;

	if ((key = ftok(SFTT_SERVER_SHMKEY_FILE, 'S')) == -1) {
		printf(PROC_NAME " ftok failed!\n"
			"\tFile \"" SFTT_SERVER_SHMKEY_FILE "\" is existed?\n");
		return -1;
	}

	int shmflag = (create_flag ? IPC_CREAT : 0) | 0666;
	if ((shmid = shmget(key, SFTT_SERVER_SHM_SIZE, shmflag)) == -1) {
		printf(PROC_NAME " shmget failed! shmflag: 0x%0x\n", shmflag);
		return -1;
	}

	return shmid;
}

void *get_sftt_server_shmaddr(int create_flag)
{
	int shmid = get_sftt_server_shmid(create_flag);
	if (shmid == -1) {
		printf(PROC_NAME " get shmid failed!\n");
		return NULL;
	}

	void *data = shmat(shmid, NULL, 0);
	if (data == (void *)(-1)) {
		printf(PROC_NAME " shmat failed!\n");
		return NULL;
	}

	return data;
}

struct sftt_server_stat *alloc_sftt_server_stat(void)
{
	return (struct sftt_server_stat *)get_sftt_server_shmaddr(1);
}

struct sftt_server_stat *get_sftt_server_stat(void)
{
	return (struct sftt_server_stat *)get_sftt_server_shmaddr(0);
}

void put_sftt_server_stat(struct sftt_server_stat *sss)
{
	shmdt(sss);
}

bool free_sftt_server_stat(void)
{
	int shmid = get_sftt_server_shmid(0);
	if (shmid == -1) {
		printf(PROC_NAME " get shmid failed!\n");
		return false;
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		printf(PROC_NAME " remove shm failed!\n");
		return false;
	};

	return true;
}

bool sftt_server_is_running(void)
{
	key_t key;
	int shmid;

	if ((key = ftok(SFTT_SERVER_SHMKEY_FILE, 'S')) == -1) {
		return false;
	}

	int shmflag = 0666;
	if ((shmid = shmget(key, SFTT_SERVER_SHM_SIZE, shmflag)) == -1) {
		return false;
	}

	struct sftt_server_stat *sss = shmat(shmid, NULL, 0);
	if (sss == (void *)(-1)) {
		return false;
	}

	return sss->status == RUNNING;
}

void update_server(struct sftt_server *server)
{
	int sock = 0;
	uint64_t current_ts = (uint64_t)time(NULL);
	int port = get_random_port();
	char buf[128];

	if (server->main_port != port) {
		sock = create_non_block_sock(&port);
		if (sock == -1) {
			return ;
		}

		sprintf(buf, "update main port and main sock. "
			"sock(%d -> %d), port(%d -> %d)", server->main_sock,
			sock, server->main_port, port);
		add_log(LOG_INFO, buf);

		if (sock != -1) {
			close(server->main_sock);
			server->main_sock = sock;
			server->main_port = port;
			server->last_update_ts = current_ts;
		}
	}
	sync_server_stat();
}

static int validate_user_info(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct validate_req *req_info;
	struct validate_resp *resp_info;
	struct user_base_info *user_base;
	struct user_auth_info *user_auth;
	int ret;
	char *md5_str;

	DEBUG((DEBUG_INFO, "handle validate req in ...\n"));

	req_info = (struct validate_req *)req_packet->obj;
	DEBUG((DEBUG_INFO, "req_info: name=%s|name_len=%d|"
		"passwd_md5=%s|passwd_len=%d|task_port=%d\n", req_info->name,
		req_info->name_len, req_info->passwd_md5,
		req_info->passwd_len, req_info->task_port));

	add_log(LOG_INFO, "receive validate request|name: %s", req_info->name);

	resp_info = mp_malloc(g_mp, __func__, sizeof(struct validate_resp));
	assert(resp_info != NULL);

	user_base = find_user_base_by_name(req_info->name);
	user_auth = find_user_auth_by_name(req_info->name);
	if (user_base == NULL) {
		resp_info->status = UVS_NTFD;
		resp_info->uid = -1;
		DEBUG((DEBUG_INFO, "cannot find user!\n"));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
	} else if (strcmp(user_auth->passwd_md5, req_info->passwd_md5)) {
		resp_info->status = UVS_INVALID;
		resp_info->uid = -1;
		DEBUG((DEBUG_INFO, "passwd not correct!\n"));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
	} else if (!file_existed(user_base->home_dir)) {
		resp_info->status = UVS_MISSHOME;
		resp_info->uid = -1;
		DEBUG((DEBUG_INFO, "cannot find user's home dir!\n"));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
	} else {
		resp_info->status = UVS_PASS;
		resp_info->uid = user_base->uid;
		client->status = ACTIVE;
		client->task_port = req_info->task_port;

		strncpy(resp_info->pwd, user_base->home_dir, DIR_PATH_MAX_LEN - 1);
		strncpy(client->pwd, user_base->home_dir, DIR_PATH_MAX_LEN - 1);

		memcpy(&client->user, user_base, sizeof(struct user_base_info));

		gen_session_id(client->session_id, SESSION_ID_LEN);
		strncpy(resp_info->session_id, client->session_id, SESSION_ID_LEN);

		DEBUG((DEBUG_INFO, "validate user info successfully!\n"));
		DEBUG((DEBUG_INFO, "user_name=%s|uid=%d|status=%d|home_dir=%s|"
			"session_id=%s\n", client->user.name, client->user.uid,
			resp_info->status, client->pwd, client->session_id));
	}
	strncpy(resp_info->name, req_info->name, USER_NAME_MAX_LEN - 1);

	resp_packet->type = PACKET_TYPE_VALIDATE_RSP;
	resp_packet->obj = resp_info;

	ret = send_sftt_packet(client->connect_fd, resp_packet);
	if (ret == -1) {
		printf("send validate response failed!\n");
		return -1;
	}

	DEBUG((DEBUG_INFO, "handle validate req out\n"));

	return 0;
}

void child_process_exception_handler(int sig)
{
	add_log(LOG_WARN, "child thread encounter seg fault!");
	printf("I'm child process and encountered segmental fault!\n");
	exit(-1);
}

void child_process_exit(int sig)
{
	add_log(LOG_INFO, "I'm child process and exit for received signal.");
	exit(-1);
}

void handle_pwd_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	struct pwd_req *req_info;
	struct pwd_resp *resp_info;
	int ret;

	DEBUG((DEBUG_INFO, "handle pwd req in ...\n"));
	req_info = req_packet->obj;
	assert(req_info != NULL);

	resp_info = mp_malloc(g_mp, __func__, sizeof(struct pwd_resp));
	assert(resp_info != NULL);

	DEBUG((DEBUG_INFO, "pwd_req: session_id=%s\n", req_info->session_id));
	if (strcmp(req_info->session_id, client->session_id)) {
		resp_info->status = SESSION_INVALID;
		resp_info->pwd[0] = 0;
	} else {
		resp_info->status = RESP_OK;
		strncpy(resp_info->pwd, client->pwd, DIR_PATH_MAX_LEN);
	}

	resp_packet->type = PACKET_TYPE_PWD_RSP;
	resp_packet->obj = resp_info;

	DEBUG((DEBUG_INFO, "pwd_resp: pwd=%s\n", resp_info->pwd));
	ret = send_sftt_packet(client->connect_fd, resp_packet);
	if (ret == -1) {
		printf("send pwd response failed!\n");
		return ;
	}

	DEBUG((DEBUG_INFO, "handle pwd req out\n"));
}

int handle_cd_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	struct cd_req *req_info;
	struct cd_resp *resp_info;
	char buf[DIR_PATH_MAX_LEN];
	int ret;

	DEBUG((DEBUG_INFO, "handle cd req in ...\n"));

	req_info = req_packet->obj;
	assert(req_info != NULL);
	DEBUG((DEBUG_INFO, "req_info: path=%s\n", req_info->path));

	resp_info = mp_malloc(g_mp, __func__, sizeof(struct cd_resp));
	assert(resp_info != NULL);

	snprintf(buf, DIR_PATH_MAX_LEN - 1, "%s/%s",
			client->pwd, req_info->path);

	simplify_path(buf);
	DEBUG((DEBUG_INFO, "change directory to: %s\n", buf));

	if (chdir(buf) || getcwd(buf, DIR_PATH_MAX_LEN - 1) == NULL) {
		resp_info->status = CANNOT_CD;
		resp_info->pwd[0] = 0;
		DEBUG((DEBUG_INFO, "change directory failed!\n"));
	} else {
		resp_info->status = RESP_OK;
		strncpy(resp_info->pwd, buf, DIR_PATH_MAX_LEN - 1);
		strncpy(client->pwd, buf, DIR_PATH_MAX_LEN - 1);
		DEBUG((DEBUG_INFO, "change directory successfully!\n"));
	}

	resp_packet->type = PACKET_TYPE_CD_RSP;
	resp_packet->obj = resp_info;
	ret = send_sftt_packet(client->connect_fd, resp_packet);
	if (ret == -1) {
		printf("send cd response failed!\n");
		return -1;
	}

	DEBUG((DEBUG_INFO, "handle cd req out\n"));

	return 0;
}

int send_ll_resp_once(struct client_session *client, struct ll_resp *resp_info,
	struct sftt_packet *resp_packet)
{
	int ret;

	resp_packet->type = PACKET_TYPE_LL_RSP;
	resp_packet->obj = resp_info;

	ret = send_sftt_packet(client->connect_fd, resp_packet);
	if (ret == -1) {
		printf("send cd response failed!\n");
		return -1;
	}

	return 0;
}

int handle_ll_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	struct ll_req *req_info;
	struct ll_resp *resp_info;
	char tmp[DIR_PATH_MAX_LEN];
	char path[DIR_PATH_MAX_LEN];
	int i = 0, j = 0, k = 0, ret = 0;
	struct dlist *file_list;
	struct dlist_node *node, *next;
	bool has_more = false;

	DEBUG((DEBUG_INFO, "handle ll req in ...\n"));

	req_info = req_packet->obj;
	assert(req_info != NULL);
	DEBUG((DEBUG_INFO, "ll_req|path=%s\n", req_info->path));

	resp_info = mp_malloc(g_mp, __func__, sizeof(struct ll_resp));
	assert(resp_info != NULL);

	if (is_abs_path(req_info->path)) {
		strncpy(path, req_info->path, DIR_PATH_MAX_LEN - 1);
	} else {
		snprintf(path, DIR_PATH_MAX_LEN - 1, "%s/%s",
			client->pwd, req_info->path);
	}

	simplify_path(path);
	DEBUG((DEBUG_INFO, "ls -l|path=%s\n", path));

	if (!file_existed(path)) {
		resp_info->nr = -1;
		resp_info->idx = -1;
		DEBUG((DEBUG_INFO, "file not existed!\n"));
		send_ll_resp_once(client, resp_info, resp_packet);

		return 0;
	}

	if (is_file(path)) {
		resp_info->nr = 1;
		resp_info->idx = -1;

		strncpy(resp_info->entries[0].name, basename(path), FILE_NAME_MAX_LEN - 1);
		resp_info->entries[0].type = FILE_TYPE_FILE;

		DEBUG((DEBUG_INFO, "list file successfully!\n"));
		send_ll_resp_once(client, resp_info, resp_packet);

		return 0;
	} else if (is_dir(path)) {
		file_list = get_top_file_list(path);
		if (file_list == NULL) {
			resp_info->nr = -1;
			resp_info->idx = -1;
			send_ll_resp_once(client, resp_info, resp_packet);

			return -1;
		}
		DEBUG((DEBUG_INFO, "list dir ...\n"));
		DEBUG((DEBUG_INFO, "file_count=%d\n", dlist_size(file_list)));
		node = dlist_head(file_list);
		while (node) {
			i = 0;
			dlist_for_each_pos(node) {
				if (i == FILE_ENTRY_MAX_CNT)
					break;
				strncpy(resp_info->entries[i].name, node->data, FILE_NAME_MAX_LEN - 1);
				snprintf(tmp, DIR_PATH_MAX_LEN - 1, "%s/%s", path, node->data);
				DEBUG((DEBUG_INFO, "file_name=%s\n", tmp));

				if (is_file(tmp))
					resp_info->entries[i].type = FILE_TYPE_FILE;
				else if (is_dir(tmp))
					resp_info->entries[i].type = FILE_TYPE_DIR;
				else
					resp_info->entries[i].type = FILE_TYPE_UNKNOWN;
				++i;
			}
			resp_info->nr = i;
			if (node) {
				resp_info->idx = k++;
				has_more = true;
			} else {
				resp_info->idx = -1;
				has_more = false;
			}
			send_ll_resp_once(client, resp_info, resp_packet);
		}
	}

	if (has_more)
		send_ll_resp_once(client, resp_info, resp_packet);

	DEBUG((DEBUG_INFO, "list file successfully!\n"));
	DEBUG((DEBUG_INFO, "handle ll req out\n"));

	return 0;
}

int send_trans_entry_by_get_resp(struct client_session *client,
	struct sftt_packet *resp_packet, struct get_resp *resp)
{
	resp_packet->type = PACKET_TYPE_GET_RSP;

	// how to serialize and deserialize properly ???
	resp_packet->obj = resp;
	resp_packet->block_size = GET_RESP_PACKET_MIN_LEN;

	int ret = send_sftt_packet(client->connect_fd, resp_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	return 0;
}

int send_file_name_by_get_resp(struct client_session *client,
	struct sftt_packet *resp_packet, char *path,
	char *fname, struct get_resp *resp)
{
	DEBUG((DEBUG_INFO, "path=%s|fname=%s\n", path, fname));
	if (is_dir(path))
		resp->entry.type = FILE_TYPE_DIR;
	else
		resp->entry.type = FILE_TYPE_FILE;

	resp->entry.mode = file_mode(path);
	strncpy(resp->entry.content, fname, FILE_NAME_MAX_LEN);
	resp->entry.len = strlen(fname);

	return send_trans_entry_by_get_resp(client, resp_packet, resp);
}

int send_file_md5_by_get_resp(struct client_session *client,
	struct sftt_packet *resp_packet, char *file,
	struct get_resp *resp)
{
	int ret;

	if (is_dir(file))
		return 0;

	resp->entry.total_size = file_size(file);

	ret = md5_file(file, resp->entry.content);
	if (ret == -1)
		return -1;

	resp->entry.len = strlen(resp->entry.content);
	DEBUG((DEBUG_INFO, "file=%s|md5=%s\n", file, resp->entry.content));

	return send_trans_entry_by_get_resp(client, resp_packet, resp);
}

int send_file_content_by_get_resp(struct client_session *client,
	struct sftt_packet *resp_packet, struct get_resp *resp)
{
	return send_trans_entry_by_get_resp(client, resp_packet, resp);
}

int send_one_file_by_get_resp(struct client_session *client,
	struct sftt_packet *resp_packet, char *path,
	char *fname, int nr, int idx)
{
	struct get_resp *resp;
	struct common_resp *com_resp;
	int ret;
	int len;
	int i = 0;
	FILE *fp;

	resp = mp_malloc(g_mp, __func__, sizeof(struct get_resp));
	assert(resp != NULL);

	resp->nr = nr;
	resp->idx = idx;
	resp->entry.idx = 0;

	if (is_dir(path))
		return send_file_name_by_get_resp(client, resp_packet, path, fname, resp);

	ret = send_file_name_by_get_resp(client, resp_packet, path, fname, resp);
	if (ret == -1)
		return -1;
	resp->entry.idx += 1;

	ret = send_file_md5_by_get_resp(client, resp_packet, path, resp);
	if (ret == -1)
		return -1;

	ret = recv_sftt_packet(client->connect_fd, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	com_resp = resp_packet->obj;
	if (com_resp->status == RESP_OK) {
		DEBUG((DEBUG_INFO, "file not changed: %s|skip ...\n", path));
		return 0;
	}

	resp->entry.idx += 1;

	fp = fopen(path, "r");
	if (fp == NULL)
		return -1;

	while (!feof(fp)) {
		ret = fread(resp->entry.content, 1, CONTENT_BLOCK_SIZE, fp);

		DEBUG((DEBUG_INFO, "send file block|len=%d\n", ret));
		resp->entry.len = ret;
		ret = send_file_content_by_get_resp(client, resp_packet, resp);
		if (ret == -1) {
			printf("send file block failed!\n");
			break;
		}

		ret = recv_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}
		com_resp = resp_packet->obj;
		if (com_resp->status != RESP_OK) {
			printf("recv response failed!\n");
			break;
		}

		resp->entry.idx += 1;
	}

	if (!feof(fp))
		ret = -1;

	fclose(fp);

	return ret;
}

#ifdef CONFIG_GET_OVERLAP
int handle_get_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct get_req *req;
	struct get_resp *resp;
	FILE *fp;
	char file[FILE_NAME_MAX_LEN];
	int ret;
	struct dlist *file_list;
	struct dlist_node *node;
	int file_count, i = 0;
	struct path_entry *entry;

	DEBUG((DEBUG_INFO, "handle get req in ...\n"));

	req = req_packet->obj;
	strncpy(file, req->path, FILE_NAME_MAX_LEN);
	DEBUG((DEBUG_INFO, "get_req: session_id=%s|path=%s\n",
		req->session_id, req->path));

	resp = mp_malloc(g_mp, __func__, sizeof(struct get_resp));
	assert(resp != NULL);

	entry = get_path_entry(file, client->pwd);

	if (!file_existed(entry->abs_path)) {
		DEBUG((DEBUG_INFO, "file not existed!\n"));
		resp->nr = -1;
		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_GET_RSP;
		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("send sftt packet failed!\n");
			return -1;
		}

		return 0;
	}

	if (is_file(entry->abs_path)) {
		DEBUG((DEBUG_INFO, "send file: %s\n", entry->abs_path));
		send_one_file_by_get_resp(client, resp_packet,
				entry->abs_path, entry->rel_path, 1, 0);
		DEBUG((DEBUG_INFO, "send file done\n"));
	} else {
		DEBUG((DEBUG_INFO, "send dir: %s\n", entry->abs_path));
		file_list = get_path_entry_list(file, client->pwd);
		file_count = dlist_size(file_list);
		DEBUG((DEBUG_INFO, "file_count: %d\n", file_count));
		dlist_for_each(file_list, node) {
			entry = node->data;
			DEBUG((DEBUG_INFO, "send %d-th file: %s\n", i, entry->abs_path));
			if (send_one_file_by_get_resp(client, resp_packet,
				entry->abs_path, entry->rel_path, file_count, i) == -1) {
				printf("send file failed: %s\n", node->data);
			}
			DEBUG((DEBUG_INFO, "send file done\n"));
			++i;
		}
		DEBUG((DEBUG_INFO, "send dir done\n"));
	}

	DEBUG((DEBUG_INFO, "handle get req out\n"));

	return 0;
}
#else
int handle_get_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{

}
#endif

int recv_one_file_by_put_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet,
	struct common_resp *com_resp, bool *has_more)
{
	char *rp = NULL;
	char file[FILE_NAME_MAX_LEN];
	char md5[MD5_STR_LEN];
	FILE *fp = NULL;
	int i = 0, ret = 0, total_size = 0;
	struct put_req *req_info = NULL;
	struct put_resp *resp_info;

	*has_more = true;

	req_info = (struct put_req *)req_packet->obj;
	assert(req_info != NULL);

	DEBUG((DEBUG_INFO, "first_idx=%d\n", req_info->entry.idx));
	DEBUG((DEBUG_INFO, "file_name=%s\n", req_info->entry.content));
	DEBUG((DEBUG_INFO, "req_info->nr=%d|req_info->idx=%d\n",
		req_info->nr, req_info->idx));
	assert(req_info->entry.idx == 0);

	resp_info = (struct put_resp *)mp_malloc(g_mp,
			__func__, sizeof(struct put_resp));
	assert(resp_info != NULL);

	rp = path_join(client->pwd, req_info->entry.content);

	DEBUG((DEBUG_INFO, "received put req|file=%s\n", rp));
	if (req_info->entry.type == FILE_TYPE_DIR) {
		if (!file_existed(rp)) {
			mkdirp(rp, req_info->entry.mode);
		}

		goto recv_one_file_done;
	}

	/* save file name */
	strncpy(file, req_info->entry.content, FILE_NAME_MAX_LEN);
	rp = path_join(client->pwd, file);

	/* recv md5 packet */
	DEBUG((DEBUG_INFO, "begin receive file md5 ...\n"));
	ret = recv_sftt_packet(client->connect_fd, req_packet);
	if (!(ret > 0)) {
		printf("recv encountered unrecoverable error ...\n");
		return -1;
	}
	req_info = req_packet->obj;

	DEBUG((DEBUG_INFO, "file total size: %d\n", req_info->entry.total_size));

	/* save md5 */
	strncpy(md5, req_info->entry.content, MD5_STR_LEN);
	if (same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "file not changed: %s\n", rp));

		/* send resp */
		com_resp->status = RESP_OK;
		resp_packet->obj = com_resp;
		resp_packet->type = PACKET_TYPE_COMMON_RSP;

		ret = send_sftt_packet(client->connect_fd, resp_packet);
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

		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}
	}

	DEBUG((DEBUG_INFO, "begin receive file content ...\n"));

	fp = fopen(rp, "w+");
	if (fp == NULL) {
		printf("create file failed: %s\n", rp);
		return -1;
	}

	i = 0;
	do {
		ret = recv_sftt_packet(client->connect_fd, req_packet);
		if (!(ret > 0)) {
			printf("recv encountered unrecoverable error ...\n");
			break;
		}
		req_info = req_packet->obj;
		DEBUG((DEBUG_INFO, "receive %d-th block file content|size=%d\n",
			(i + 1), req_info->entry.len));

		fwrite(req_info->entry.content, req_info->entry.len, 1, fp);

		/* send response */
		resp_info->status = RESP_OK;
		resp_packet->obj = resp_info;
		resp_packet->type = PACKET_TYPE_PUT_RSP;

		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("send put response failed!\n");
			break;
		}

		total_size += req_info->entry.len;
		i += 1;
	} while (total_size < req_info->entry.total_size);

	fclose(fp);

	if (total_size == req_info->entry.total_size) {
		DEBUG((DEBUG_INFO, "received one file: %s\n", rp));
	} else {
		DEBUG((DEBUG_INFO, "receive file failed: %s\n", rp));
		return -1;
	}

	if (!same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "recv one file failed: %s|"
			"md5 not correct!\n", rp));
		return -1;
	}

recv_one_file_done:
	if (req_info->idx == req_info->nr - 1)
		*has_more = false;

	set_file_mode(rp, req_info->entry.mode);

	DEBUG((DEBUG_INFO, "recv %s done!\n", rp));

	return 0;
}

int handle_put_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct common_resp *com_resp;
	int ret, i = 0;
	bool has_more = true;

	DEBUG((DEBUG_INFO, "handle put req in ...\n"));
	com_resp = (struct common_resp *)mp_malloc(g_mp, __func__,
			sizeof(struct common_resp));
	assert(com_resp != NULL);

	do {
		DEBUG((DEBUG_INFO, "recv %d-th file ...\n", i));
		ret = recv_one_file_by_put_req(client, req_packet, resp_packet,
			com_resp, &has_more);
		if (ret == -1 || has_more == false)
			break;

		ret = recv_sftt_packet(client->connect_fd, req_packet);
		if (!(ret > 0)) {
			printf("recv encountered unrecoverable error ...\n");
			break;
		}
		++i;
	} while (has_more);

	DEBUG((DEBUG_INFO, "handle put req out\n"));

	return ret;
}

int handle_directcmd_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct directcmd_req *req;
	struct directcmd_resp *resp;
	char cmd[CMD_MAX_LEN + FILE_NAME_MAX_LEN];
	char temp_file[FILE_NAME_MAX_LEN];
	int ret;
	long total_len;
	FILE *fp;

	DEBUG((DEBUG_INFO, "handle directcmd req in ...\n"));
	resp = (struct directcmd_resp *)mp_malloc(g_mp, __func__,
			sizeof(struct directcmd_resp));
	assert(resp != NULL);

	req = req_packet->obj;
	assert(req != NULL);

	ret = create_temp_file(temp_file, "sfttd_directcmd_");
	if (ret == -1) {
		DEBUG((DEBUG_INFO, "cannot create temp file!\n"));
		resp->total_len = -1;
		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_DIRECTCMD_RSP;

		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("send mp stat response failed!\n");
		}
		return -1;
	}

	snprintf(cmd, sizeof(cmd), "%s > %s", req->cmd, temp_file);
	DEBUG((DEBUG_INFO, "%s\n", cmd));
	system(cmd);
	resp->total_len = file_size(temp_file);

	fp = fopen(temp_file, "rb");
	if (fp == NULL) {
		DEBUG((DEBUG_INFO, "open command result file failed!\n"));
		resp->total_len = -1;
		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_DIRECTCMD_RSP;

		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("send mp stat response failed!\n");
		}

		return -1;
	}

	total_len = 0;
	do {
		ret = fread(resp->data, 1, CMD_RET_BATCH_LEN - 1, fp);
		resp->this_len = ret;
		total_len += resp->this_len;

		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_DIRECTCMD_RSP;

		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("send mp stat response failed!\n");
		}
	} while (total_len < resp->total_len);

	unlink(temp_file);

	return 0;
}

int handle_mp_stat_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct mp_stat_resp *resp;
	struct mem_pool_stat stat;
	int ret, i = 0;

	DEBUG((DEBUG_INFO, "handle mempool stat req in ...\n"));
	resp = (struct mp_stat_resp *)mp_malloc(g_mp, __func__,
			sizeof(struct mp_stat_resp));
	assert(resp != NULL);

	get_mp_stat(g_mp, &stat);
	resp->total_size = stat.total_size;
	resp->total_nodes = stat.total_nodes;
	resp->using_nodes = stat.using_nodes;
	resp->free_nodes = stat.free_nodes;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_MP_STAT_RSP;

	ret = send_sftt_packet(client->connect_fd, resp_packet);
	if (ret == -1) {
		printf("send mp stat response failed!\n");
	}

	DEBUG((DEBUG_INFO, "handle mempool stat req out\n"));

	return ret;
}

struct logged_in_user *get_logged_in_users(int *count)
{
	int i, j;
	struct logged_in_user *users = NULL;
	struct client_session *session;

	server->pm->ops->lock(server->pm);

	for (i = 0, j = 0; i < MAX_CLIENT_NUM; ++i) {
		session = &server->sessions[i];
		if (client_connected(session)) {
			++j;
		}
	}

	*count = j;
	if (j == 0)
		goto done;

	users = mp_malloc(g_mp, __func__, sizeof(struct logged_in_user) * (*count));
	if (users == NULL) {
		*count = -1;
		users = NULL;
		goto done;
	}

	for (i = 0, j = 0; i < MAX_CLIENT_NUM; ++i) {
		session = &server->sessions[i];
		if (client_connected(session)) {
			strncpy(users[j].session_id, session->session_id, SESSION_ID_LEN - 1);
			strncpy(users[j].name, session->user.name, USER_NAME_MAX_LEN - 1);
			strncpy(users[j].ip, session->ip, IPV4_MAX_LEN - 1);
			users[j].port = session->port;
			users[j].task_port = session->task_port;
			++j;
		}
	}

done:
	server->pm->ops->unlock(server->pm);

	return users;
}

int handle_who_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct who_resp *resp;
	struct logged_in_user *users;
	int total, num, count;
	int ret, i = 0;

	DEBUG((DEBUG_INFO, "handle who req in ...\n"));
	resp = (struct who_resp *)mp_malloc(g_mp, __func__,
			sizeof(struct who_resp));

	if (resp == NULL) {
		DEBUG((DEBUG_INFO, "cannot alloc memory for resp!\n"));
		return -1;
	}

	users = get_logged_in_users(&total);
	DEBUG((DEBUG_INFO, "There has %d user(s) logged in\n", total));
	if (users == NULL || total <= 0) {
		DEBUG((DEBUG_INFO, "get logged in user failed\n"));
		resp->total = -1;
		resp->num = 0;

		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_WHO_RSP;

		send_sftt_packet(client->connect_fd, resp_packet);

		return 0;
	}

	count = 0;
	do {
		num = (total - count) >= LOGGED_IN_USER_MAX_CNT ?
			LOGGED_IN_USER_MAX_CNT : (total - count);
		resp->total = total;
		resp->num = num;
		for (i = 0; i < num; ++i) {
			bzero(&resp->users[i], sizeof(struct logged_in_user));
			strncpy(resp->users[i].session_id, users[i + count].session_id,
					SESSION_ID_LEN - 1);
			strncpy(resp->users[i].ip, users[i + count].ip,
					IPV4_MAX_LEN - 1);
			strncpy(resp->users[i].name, users[i + count].name,
					USER_NAME_MAX_LEN - 1);
			resp->users[i].port = users[i + count].port;
			resp->users[i].task_port = users[i + count].task_port;
		}

		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_WHO_RSP;

		send_sftt_packet(client->connect_fd, resp_packet);

		count += num;
	} while (count < total);

	DEBUG((DEBUG_INFO, "handle who req out ...\n"));

	return ret;
}

int check_user(struct logged_in_user *user)
{
	int i, j;
	int ret = -1;
	struct logged_in_user *users;
	struct client_session *session;

	server->pm->ops->lock(server->pm);

	for (i = 0, j = 0; i < MAX_CLIENT_NUM; ++i) {
		session = &server->sessions[i];
		printf("%s:%d, session->session_id=%s, user->session_id=%s\n",
				__func__, __LINE__, session->session_id,
				user->session_id);
		if (client_connected(session) &&
			strcmp(session->session_id, user->session_id) == 0) {
			ret = 0;
			goto done;
		}
	}

done:
	server->pm->ops->unlock(server->pm);

	return ret;
}

int handle_write_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct write_req *req;
	struct logged_in_user *user;
	struct peer_session *peer = NULL;
	struct peer_session *pos = NULL;
	int ret;

	req = req_packet->obj;
	user = &req->user;
	if (check_user(user) == -1) {
		printf("user not found!\n");
		return -1;
	}

	list_for_each_entry(pos, &client->peers, list)
		if (strcmp(pos->session_id, user->session_id) == 0) {
			peer = pos;
			break;
		}

	if (peer == NULL) {
		peer = mp_malloc(g_mp, "new_peer_session", sizeof(struct peer_session));
		if (peer == NULL) {
			printf("cannot alloc peer session!\n");
			return -1;
		}

		strncpy(peer->session_id, user->session_id, SESSION_ID_LEN - 1);

		peer->connect_fd = make_connect(user->ip, user->task_port);
		if (peer->connect_fd == -1) {
			printf("cannot make connect to peer!\n");
			return -1;
		}

		list_add(&peer->list, &client->peers);
	}

	ret = send_sftt_packet(peer->connect_fd, req_packet);
	if (ret == -1) {
		printf("send write message to peer failed!\n");
		return -1;
	}

	return 0;
}

void *handle_client_session(void *args)
{
	struct client_session *client = (struct client_session *)args;
	int sock = client->connect_fd;
	struct sftt_packet *resp;
	struct sftt_packet *req;
	int ret;

	DEBUG((DEBUG_INFO, "begin handle client session ...\n"));
	req = malloc_sftt_packet(REQ_PACKET_MIN_LEN);
	if (!req) {
		printf("cannot allocate resources from memory pool!\n");
		return NULL;
	}

	//DEBUG((DEBUG_INFO, "normal"));
	signal(SIGTERM, child_process_exit);
	signal(SIGSEGV, child_process_exception_handler);

	//if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
	//	printf("set sockfd to non-block failed!\n");
	//		return -1;
	//}

	add_log(LOG_INFO, "begin to communicate with client ...");
	//DEBUG((DEBUG_INFO, "normal"));
	while (1) {
		ret = recv_sftt_packet(sock, req);
		//DEBUG((DEBUG_INFO, "normal"));
		add_log(LOG_INFO, "recv ret: %d", ret);
		if (ret == -1) {
			printf("recv encountered unrecoverable error, child process is exiting ...\n");
			goto exit;
		}
		if (ret == 0) {
			add_log(LOG_INFO, "client disconnected, child process is exiting ...");
			goto exit;
		}
		switch (req->type) {
		case PACKET_TYPE_VALIDATE_REQ:
			resp = malloc_sftt_packet(VALIDATE_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}

			ret = validate_user_info(client, req, resp);
			if (ret == -1) {
				free_sftt_packet(&resp);
				goto exit;
			}
			break;
		case PACKET_TYPE_PWD_REQ:
			resp = malloc_sftt_packet(PWD_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_pwd_req(client, req, resp);
			break;
		case PACKET_TYPE_CD_REQ:
			resp = malloc_sftt_packet(CD_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_cd_req(client, req, resp);
			break;
		case PACKET_TYPE_LL_REQ:
			resp = malloc_sftt_packet(LL_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_ll_req(client, req, resp);
			break;
		case PACKET_TYPE_PUT_REQ:
			resp = malloc_sftt_packet(PUT_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_put_req(client, req, resp);
			break;
		case PACKET_TYPE_GET_REQ:
			resp = malloc_sftt_packet(GET_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_get_req(client, req, resp);
			break;
		case PACKET_TYPE_MP_STAT_REQ:
			resp = malloc_sftt_packet(MP_STAT_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_mp_stat_req(client, req, resp);
			break;
		case PACKET_TYPE_DIRECTCMD_REQ:
			resp = malloc_sftt_packet(DIRECTCMD_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_directcmd_req(client, req, resp);
			break;
		case PACKET_TYPE_WHO_REQ:
			resp = malloc_sftt_packet(WHO_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_who_req(client, req, resp);
			break;
		case PACKET_TYPE_WRITE_REQ:
			resp = malloc_sftt_packet(WRITE_RESP_PACKET_MIN_LEN);
			if (resp == NULL) {
				goto exit;
			}
			handle_write_req(client, req, resp);
			break;
		default:
			printf("%s: cannot recognize packet type!\n", __func__);
			break;
		}
		free_sftt_packet(&resp);
	}

exit:
	client->status = DISCONNECTED;
	DEBUG((DEBUG_INFO, "a client is disconnected\n"));
	return NULL;
}

void sync_server_stat(void)
{
	struct sftt_server_stat *sss = get_sftt_server_stat();
	if (sss == NULL) {
		printf("get " PROC_NAME " info object failed!\n");
		return ;
	}

	sss->main_sock = server->main_sock;
	sss->main_port = server->main_port;
	sss->last_update_ts = server->last_update_ts;
	sss->status = server->status;

	put_sftt_server_stat(sss);
}

struct client_session *find_get_new_session(void)
{
	int i;

	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		if (!client_connected(&server->sessions[i])) {
			set_client_active(&server->sessions[i]);
			return &server->sessions[i];
		}
	}

	return NULL;
}

void init_client_session(struct client_session *session)
{
	set_client_disconnected(session);

	INIT_LIST_HEAD(&session->peers);
}

void init_sessions(void)
{
	int i = 0;
	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		init_client_session(&server->sessions[i]);
	}
}

void main_loop(void)
{
	int connect_fd = 0;
	pid_t pid = 0;
	int idx = 0;
	int ret;
	pthread_t child;
	struct client_session *session;
	struct sockaddr_in addr_client;
	int len;

	server->status = RUNNING;
	init_sessions();

	//DEBUG((DEBUG_INFO, "normal"));

	len = sizeof(struct sockaddr_in);
	while (1) {
		update_server(server);
		connect_fd = accept(server->main_sock, (struct sockaddr *)&addr_client, &len);
		if (connect_fd == -1) {
			usleep(100 * 1000);
			continue;
		}
		if ((session = find_get_new_session()) == NULL) {
			add_log(LOG_INFO, "exceed max connection!");
			close(connect_fd);
			continue;
		}
		add_log(LOG_INFO, "a client connected ...");
		session->connect_fd = connect_fd;

		bzero(session->ip, IPV4_MAX_LEN - 1);
		strncpy(session->ip, inet_ntoa(addr_client.sin_addr), IPV4_MAX_LEN - 1);
		session->port = ntohs(addr_client.sin_port);

		DEBUG((DEBUG_INFO, "a client is connecting ...\n"));
		DEBUG((DEBUG_INFO, "ip=%s|port=%d\n", session->ip, session->port));

		ret = pthread_create(&child, NULL, handle_client_session, session);
		if (ret) {
			add_log(LOG_INFO, "create thread failed!");
			session->status = EXITED;
		}
	}
}

int create_non_block_sock(int *pport)
{
	int	sockfd;
	struct sockaddr_in serveraddr;
	int rand_port = get_random_port();
	add_log(LOG_INFO, "random port is %d", rand_port);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("create socket filed");
		return -1;
	}

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
		perror("set sockfd to non-block failed");
		return -1;
	}

	memset(&serveraddr, 0 ,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(rand_port);

	if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		perror("bind socket error");
		return -1;
	}

	if (listen(sockfd, 10) == -1){
		perror("listen socket error");
		return -1;
	}

	if (pport) {
		*pport = rand_port;
	}

	return sockfd;
}

struct database *start_sftt_db_server(void)
{
	return (void *)-1;
}

int start_sftt_log_server(struct sftt_server *server)
{
	int ret;

	set_log_type(SERVER_LOG);

	strncpy(logger_ctx.dir, server->conf.log_dir, DIR_PATH_MAX_LEN - 1);
	strncpy(logger_ctx.prefix, PROC_NAME, LOGGER_PREFIX_LEN - 1);

	ret = pthread_create(&server->log_tid, NULL, logger_daemon, &logger_ctx);
	if (ret)
		return -1;

	return 0;
}

int init_sftt_server(char *store_path)
{
	int port = 0;
	int sockfd;
	int ret;

#if 0
	char tmp_file[32];
	if (create_temp_file(tmp_file, "sfttd_") == -1)
		return -1;
#endif
	set_current_context("server");

	sockfd = create_non_block_sock(&port);
	if (sockfd == -1) {
		return -1;
	}

	server = (struct sftt_server *)mp_malloc(g_mp, __func__, sizeof(struct sftt_server));
	assert(server != NULL);
	server->status = READY;
	server->main_sock = sockfd;
	server->main_port = port;
	server->last_update_ts = (uint64_t)time(NULL);
	if (get_sftt_server_config(&(server->conf)) == -1) {
		return -1;
	}

	if (strlen(store_path)) {
		strcpy(server->conf.store_path, store_path);
	}

#if 0
	if ((server->db = start_sftt_db_server()) == NULL) {
		printf("cannot start " PROC_NAME " database!\n");
		return false;
	}
#endif

	ret = start_sftt_log_server(server);
	if (ret == -1) {
		printf("cannot start log server!\n");
		return -1;
	}

	ret = init_sftt_server_stat();
	if (ret == -1) {
		printf(PROC_NAME " start failed! Because cannot init "
			PROC_NAME " server info.\n");
		return -1;
	}
	server->pm = new(pthread_mutex);

	return 0;
}

int sftt_server_start(char *store_path, bool background)
{
	if (sftt_server_is_running()) {
		printf("cannot start " PROC_NAME ", because it has been running.\n");
		exit(-1);
	}

	if (strlen(store_path)) {
		if (access(store_path, W_OK)) {
			printf("write %s: Operation not permitted!\n", store_path);
			exit(-1);
		}
	}

	if (background && daemon(1, 1) != 0) {
		printf("server cannot running in the background!\n");
		exit(-1);
	}

	if (init_sftt_server(store_path) == -1) {
		printf(PROC_NAME " create server failed!\n");
		exit(-1);
	}

	add_log(LOG_INFO, PROC_NAME " is going to start in the background ...");

	signal(SIGTERM, sftt_server_exit);

	main_loop();
}

int sftt_server_restart(char *store_path, bool background)
{
#if 0
	if (!sftt_server_is_running()) {
		printf("cannot restart " PROC_NAME ", because it is not running.\n");
		exit(-1);
	}
#endif

	if (strlen(store_path)) {
		if (access(store_path, W_OK)) {
			printf("write %s: Operation not permitted!\n", store_path);
			exit(-1);
		}
	}

	sftt_server_stop();
	sftt_server_start(store_path, background);

	add_log(LOG_INFO, PROC_NAME " is going to restart ...");

	return 0;
}

/**
 * we should wait all children exit before sftt server exits.
 * todo: fix this bug.
 **/
int sftt_server_stop(void)
{
	if (!sftt_server_is_running()) {
		printf("cannot stop " PROC_NAME ", because it is not running.\n");
		exit(-1);
	}

	struct sftt_server_stat *sss = get_sftt_server_stat();
	if (sss == NULL) {
		printf("cannot get " PROC_NAME " info!\n");
		return -1;
	}

	logger_exit(SIGTERM);

	/**
	*	All  of  these  system calls are used to wait for state changes
	*	in a child of the calling process, and obtain information about
	*	the child whose state has changed. A state change is considered
	*	to be: the child terminated; the child was stopped by a signal;
	*	or the child was resumed by a signal. In the case of a terminated
	*	child, performing a wait allows the system to release the resources
	*   	associated with the child; if a wait is not performed, then
	*	the terminated child remains in a "zombie" state (see NOTES below).
	*
	*	If  a  child  has  already changed state, then these calls return
	*	immediately. Otherwise they block until either a child changes state
	*	or a signal handler interrupts the call (assuming that system calls
	*	are not automatically restarted using the SA_RESTART flag of sigaction(2)).
	*	In the remainder of this page, a child whose state has changed and
	*	which has not yet been waited upon by one of these system calls
	*	is termed waitable.
	*
	**/
	printf(PROC_NAME " pid is: %d\n", sss->main_pid);
	printf(PROC_NAME " is going to stop ...\n");
	kill(sss->main_pid, SIGTERM);
	free_sftt_server_stat();

	return 0;
}

void notify_all_child_to_exit(void)
{
	int i = 0;
	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		if (server->sessions[i].status != ACTIVE) {
			continue;
		}
	}
}

void sftt_server_exit(int sig)
{
	printf(PROC_NAME " is exit ...!\n");
	notify_all_child_to_exit();
	free_sftt_server_stat();

	exit(-1);
}

void server_usage_help(int exitcode)
{
	version();
	printf("usage:\t" PROC_NAME " options\n"
		"\t" PROC_NAME " start [-d] [-s dir]\n"
		"\t" PROC_NAME " restart [-d] [-s dir]\n"
		"\t" PROC_NAME " stop\n"
		"\t" PROC_NAME " status\n"
		"\t" PROC_NAME " db\n");
	exit(exitcode);
}

bool parse_store_path(char *optarg, char *store_path, int max_len)
{
	if (!optarg || !store_path) {
		return false;
	}

	strncpy(store_path, optarg, max_len);

	return true;
}

const char *status_desc(enum sftt_server_status status)
{
	switch (status) {
	case SERVERING:
		return "running";
	case STOPED:
		return "not running";
	default:
		return "unknown";
	}
}

void sftt_server_status(void)
{
	if (!sftt_server_is_running()) {
		printf(PROC_NAME " is not running.\n");
		return ;
	}

	struct sftt_server_stat *sss = get_sftt_server_stat();
	if (sss == NULL) {
		printf("cannot get " PROC_NAME " status!\n");
		return ;
	}

	char ts_buf[64];
	ts_to_str(sss->last_update_ts, ts_buf, 63);

	printf(PROC_NAME " status:\n"
		"\tprocess status: %s\n"
		"\tstore path: %s\n"
		"\tmain pid: %d\n"
		"\tmain port: %d\n"
		"\tmain sock: %d\n"
		"\tlog path: %s\n"
		"\tlast update time: %s\n",
		status_desc(sss->status),
		sss->conf.store_path,
		sss->main_pid,
		sss->main_port,
		sss->main_sock,
		sss->conf.log_dir,
		ts_buf);
}

char *fetch_next_str(char **str)
{
	char *p, *q;

	if (str == NULL || *str == NULL)
		return NULL;

	p = *str;
	/* skip blank */
	while (*p && (*p == ' ' || *p == '\t'))
		++p;
	if (!*p)
		return NULL;

	q = p;
	while (*q && (*q != ' ' && *q != '\t'))
		++q;
	if (*q) {
		*q = 0;
		*str = q + 1;
	} else {
		*str = q;
	}

	return p;
}

void db_add_user(struct db_connect *db_con, char *info)
{
	char *name, *passwd;
	char passwd_md5[PASSWD_MD5_LEN];

	name = fetch_next_str(&info);
	if (name == NULL || strlen(name) == 0) {
		printf("bad format\n");
		return ;
	}

	passwd = fetch_next_str(&info);
	if (passwd == NULL || strlen(passwd) == 0) {
		printf("bad format\n");
		return ;
	}
	printf("passwd: %s\n", passwd);

	if (find_user_base_by_name(name)) {
		printf("user %s has already existed!\n", name);
		return ;
	}

	md5_str(passwd, strlen(passwd), passwd_md5);

	if (user_add(name, passwd_md5) == -1) {
		printf("add user failed!\n");
		return ;
	}

	printf("add %s successfully!\n", name);
}

void db_update_user(struct db_connect *db_con, char *info)
{
	char *name, *key_name, *value;
	char passwd_md5[PASSWD_MD5_LEN];

	name = fetch_next_str(&info);
	if (name == NULL || strlen(name) == 0) {
		printf("bad format\n");
		return ;
	}

	key_name = fetch_next_str(&info);
	if (key_name == NULL || strlen(key_name) == 0) {
		printf("bad format\n");
		return ;
	}

	value = fetch_next_str(&info);
	if (value == NULL || strlen(value) == 0) {
		printf("bad format\n");
		return ;
	}

	if (find_user_base_by_name(name) == NULL) {
		printf("user %s dosen't existed!\n", name);
		return ;
	}

	if (strcmp(key_name, "passwd") == 0) {
		md5_str(value, strlen(value), passwd_md5);
		update_user_base_info(name, "passwd_md5", passwd_md5);
	} else if (strcmp(key_name, "home_dir") == 0 ||
		   strcmp(key_name, "create_time") == 0) {
		update_user_base_info(name, key_name, value);
	} else {
		printf("cannot recognize key name: %s\n", key_name);
	}

	return ;
}

void db_user_info(struct db_connect *db_con, char *cmd)
{
	struct user_base_info *user_base = NULL;
	char ctime_buf[32];
	char uptime_buf[32];
	char *name = NULL;

	name = fetch_next_str(&cmd);
	if (name == NULL || strlen(name) == 0) {
		printf("bad format\n");
		return ;
	}

	if ((user_base = find_user_base_by_name(name)) == NULL) {
		printf("user %s not found!\n", name);
		return ;
	}

	ts_to_str(user_base->create_time, ctime_buf, 31);
	ts_to_str(user_base->update_time, uptime_buf, 31);
	printf("name: %s, uid: %d, home: %s, create time: %s, "
		"last update: %s\n", user_base->name, user_base->uid,
		user_base->home_dir, ctime_buf, uptime_buf);
}

void execute_db_cmd(struct db_connect *db_con, char *cmd, int flag)
{
	int len = strlen(cmd);
	char *p;

	p = fetch_next_str(&cmd);
	if (p == NULL) {
		printf("cannot parse command\n");
		return ;
	}

	if (strcmp(p, "adduser") == 0) {
		db_add_user(db_con, cmd);
	} else if (strcmp(p, "update") == 0){
		db_update_user(db_con, cmd);
	} else if (strcmp(p, "info") == 0) {
		db_user_info(db_con, cmd);
	} else {
		printf("unknown command: %s\n", p);
	}

}

void sftt_server_db(void)
{
	char cmd[1024];
	struct db_connect *db_con;
	char *user_db_file = get_user_db_file();

	if (user_db_file == NULL) {
		printf("cannot get db file!\n");
		return ;
	}
	printf("user db file is: %s\n", user_db_file);

	db_con = create_db_connect(user_db_file);
	if (db_con == NULL) {
		printf("cannot connect to db!\n");
		return ;
	}

	while (1) {
		printf("sftt_db>> ");
		fgets(cmd, 1024, stdin);
		cmd[strlen(cmd) - 1] = 0;
		if (!strcmp(cmd, "quit")) {
			exit(0);
		} else {
			execute_db_cmd(db_con, cmd, -1);
		}
	}
}

