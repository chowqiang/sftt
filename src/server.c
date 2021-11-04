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
#include "file_trans.h"
#include "lock.h"
#include "log.h"
#include "mem_pool.h"
#include "mkdirp.h"
#include "net_trans.h"
#include "endpoint.h"
#include "req_resp.h"
#include "response.h"
#include "server.h"
#include "session.h"
#include "trans.h"
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
extern int verbose_level;

struct sftt_server *server;

struct logger_init_ctx logger_ctx;

int create_non_block_sock(int *pport);

void put_sftt_server_stat(struct sftt_server_stat *sss);

struct sftt_server_stat *alloc_sftt_server_stat(void);

void sync_server_stat(void);

int check_user(struct logged_in_user *user);

void put_session(struct client_session *s);

struct client_session *find_client_session_by_id(char *session_id);

void put_peer_task_conn(struct client_sock_conn *conn);

struct client_sock_conn *get_peer_task_conn(struct logged_in_user *user);

void init_client_session(struct client_session *session);

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

int server_consult_block_size(int connect_fd, unsigned char *buff,int server_block_size)
{
	int trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
	if (trans_len <= 0 ) {
		printf("consult block size recv failed!\n");
		exit(0);
	}

	//char *mybuff = sftt_decrypt_func(buff,trans_len);
	int client_block_size = atoi((char *)buff);
	int min_block_size = client_block_size < server_block_size ?
		client_block_size : server_block_size;

	sprintf((char *)buff,"%d",min_block_size);
	int size = strlen((char *)buff);
	sftt_encrypt_func(buff,size);
	send(connect_fd,buff,BUFFER_SIZE,0);

	return min_block_size;

}

void server_file_resv(int connect_fd, int consulted_block_size,
	struct sftt_server_config init_conf)
{
	struct sftt_packet *sp = malloc_sftt_packet();
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
			if (sp->data_len <= 0) {
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
	//int i;
	FILE * fd;
	data_buff = strcat(data_buff,init_conf.store_path);
	strcat(data_buff, (char *)sp->content);
			
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
	//int	trans_len;
	pid_t   pid;
	unsigned char	buff[BUFFER_SIZE] = {'\0'};
	//char    quit[BUFFER_SIZE] = {'q','u','i','t'};
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

	return sss->status == SERVERING;
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
	struct validate_req *req;
	struct validate_resp *resp;
	struct validate_resp_data *resp_data;
	struct user_base_info *user_base;
	struct user_auth_info *user_auth;
	//char *md5_str;
	int ret;

	DEBUG((DEBUG_INFO, "handle validate req in ...\n"));

	req = (struct validate_req *)req_packet->obj;
	DEBUG((DEBUG_INFO, "req: name=%s|name_len=%d|"
		"passwd_md5=%s|passwd_len=%d\n", req->name,
		req->name_len, req->passwd_md5, req->passwd_len));

	add_log(LOG_INFO, "receive validate request|name: %s", req->name);

	resp = mp_malloc(g_mp, __func__, sizeof(struct validate_resp));
	assert(resp != NULL);
	resp_data = &resp->data;

	if (check_version(&req->ver, &server->ver, resp->message,
		RESP_MESSAGE_MAX_LEN - 1) == -1) {
		return send_validate_resp(client->main_conn.sock, resp_packet,
				resp, RESP_UVS_BAD_VER, 0);
	}

	user_base = find_user_base_by_name(req->name);
	user_auth = find_user_auth_by_name(req->name);
	if (user_base == NULL) {
		DEBUG((DEBUG_INFO, "cannot find user!\n"));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
		return send_validate_resp(client->main_conn.sock, resp_packet,
			       resp, RESP_UVS_NTFD, 0);	
	} else if (strcmp(user_auth->passwd_md5, req->passwd_md5)) {
		DEBUG((DEBUG_INFO, "passwd not correct!\n"));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
		return send_validate_resp(client->main_conn.sock, resp_packet,
				resp, RESP_UVS_INVALID, 0);
	} else if (!file_existed(user_base->home_dir)) {
		DEBUG((DEBUG_INFO, "cannot find user's home dir!\n"));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
		return send_validate_resp(client->main_conn.sock, resp_packet,
				resp, RESP_UVS_MISSHOME, 0);
	} else {
		client->status = ACTIVE;
		strncpy(client->pwd, user_base->home_dir, DIR_PATH_MAX_LEN - 1);
		memcpy(&client->user, user_base, sizeof(struct user_base_info));
		gen_session_id(client->session_id, SESSION_ID_LEN);

		resp->status = RESP_UVS_PASS;
		resp->next = 0;

		resp_data->uid = user_base->uid;
		strncpy(resp_data->name, req->name, USER_NAME_MAX_LEN - 1);
		strncpy(resp_data->pwd, user_base->home_dir, DIR_PATH_MAX_LEN - 1);
		strncpy(resp_data->session_id, client->session_id, SESSION_ID_LEN);

		DEBUG((DEBUG_INFO, "validate user info successfully!\n"));
		DEBUG((DEBUG_INFO, "user_name=%s|uid=%ld|status=%d|home_dir=%s|"
			"session_id=%s\n", client->user.name, client->user.uid,
			resp->status, client->pwd, client->session_id));
	}

	resp_packet->type = PACKET_TYPE_VALIDATE_RESP;
	resp_packet->obj = resp;

	ret = send_sftt_packet(client->main_conn.sock, resp_packet);
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

int handle_pwd_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	struct pwd_req *req;
	struct pwd_resp *resp;
	struct pwd_resp_data *data;
	int ret;

	DEBUG((DEBUG_INFO, "handle pwd req in ...\n"));
	req = req_packet->obj;
	assert(req != NULL);

	resp = mp_malloc(g_mp, __func__, sizeof(struct pwd_resp));
	assert(resp != NULL);
	data = &resp->data;

	DEBUG((DEBUG_INFO, "pwd_req: session_id=%s\n", req->session_id));
	if (strcmp(req->session_id, client->session_id)) {
		return send_pwd_resp(client->main_conn.sock, resp_packet,
				resp, RESP_SESSION_INVALID, 0);
	} else {
		resp->status = RESP_OK;
		strncpy(data->pwd, client->pwd, DIR_PATH_MAX_LEN);
	}

	resp_packet->type = PACKET_TYPE_PWD_RESP;
	resp_packet->obj = resp;

	DEBUG((DEBUG_INFO, "pwd_resp: pwd=%s\n", data->pwd));
	ret = send_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("send pwd response failed!\n");
		return -1;
	}

	DEBUG((DEBUG_INFO, "handle pwd req out\n"));

	return 0;
}

int handle_cd_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	struct cd_req *req;
	struct cd_resp *resp;
	struct cd_resp_data *data;
	char buf[2 * DIR_PATH_MAX_LEN + 1];
	int ret;

	DEBUG((DEBUG_INFO, "handle cd req in ...\n"));

	req = req_packet->obj;
	assert(req != NULL);
	DEBUG((DEBUG_INFO, "req: path=%s\n", req->path));

	resp = mp_malloc(g_mp, __func__, sizeof(struct cd_resp));
	assert(resp != NULL);
	data = &resp->data;

	snprintf(buf, sizeof(buf) - 1, "%s/%s",
			client->pwd, req->path);

	simplify_path(buf);
	DEBUG((DEBUG_INFO, "change directory to: %s\n", buf));

	if (chdir(buf) || getcwd(buf, DIR_PATH_MAX_LEN - 1) == NULL) {
		DEBUG((DEBUG_INFO, "change directory failed!\n"));
		return send_cd_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CANNOT_CD, 0);
	} else {
		resp->status = RESP_OK;
		strncpy(data->pwd, buf, DIR_PATH_MAX_LEN - 1);
		strncpy(client->pwd, buf, DIR_PATH_MAX_LEN - 1);
		DEBUG((DEBUG_INFO, "change directory successfully!\n"));
	}

	resp_packet->type = PACKET_TYPE_CD_RESP;
	resp_packet->obj = resp;
	ret = send_sftt_packet(client->main_conn.sock, resp_packet);
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

	resp_packet->type = PACKET_TYPE_LL_RESP;
	resp_packet->obj = resp_info;

	ret = send_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("send cd response failed!\n");
		return -1;
	}

	return 0;
}

int handle_fwd_ll_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	int ret;
	struct logged_in_user *user;
	struct peer_session *peer;
	struct ll_req *req;
	struct ll_resp *resp;
	struct client_sock_conn *conn = NULL;

	resp = mp_malloc(g_mp, __func__, sizeof(struct ll_resp));
	assert(resp != NULL);

	// check user info
	req = req_packet->obj;
	user = &req->user;
	if (check_user(user) == -1) {
		return send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_CHECK_USER, 0);
	}

	// get or create peer task conn
	conn = get_peer_task_conn(user);
	if (conn == NULL) {
		return send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_GET_PEER, 0);
	}

	// send ll req packet to peer task conn
	ret = send_sftt_packet(conn->sock, req_packet);
	if (ret == -1) {
		DEBUG((DEBUG_INFO, "send ll req to peer failed!\n"));
		ret = send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_SEND_PEER_ERR, 0);
		goto done;
	}

	do {
		// recv ll resp packet
		ret = recv_sftt_packet(conn->sock, resp_packet);
		if (ret == -1) {
			DEBUG((DEBUG_INFO, "recv sftt packet failed!\n"));
			break;
		}

		resp = (struct ll_resp *)resp_packet->obj;
		assert(resp != NULL);

		send_ll_resp_once(client, resp, resp_packet);

	} while (resp->next);

done:
	if (conn)
		put_peer_task_conn(conn);

	return ret;
}

int handle_ll_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	struct ll_req *req;
	struct ll_resp *resp;
	struct ll_resp_data *data;
	char tmp[2 * DIR_PATH_MAX_LEN + 4];
	char path[2 * DIR_PATH_MAX_LEN + 2];
	int i = 0;
	struct dlist *file_list;
	struct dlist_node *node;
	bool has_more = false;

	DEBUG((DEBUG_INFO, "handle ll req in ...\n"));

	req = req_packet->obj;
	assert(req != NULL);

	DEBUG((DEBUG_INFO, "ll_req|path=%s\n", req->path));

	resp = mp_malloc(g_mp, __func__, sizeof(struct ll_resp));
	assert(resp != NULL);
	data = &resp->data;

	// if the ll req is to peer?
	if (req->to_peer)
		return handle_fwd_ll_req(client, req_packet, resp_packet);

	if (is_absolute_path(req->path)) {
		strncpy(path, req->path, DIR_PATH_MAX_LEN - 1);
	} else {
		snprintf(path, sizeof(path) - 1, "%s/%s",
			client->pwd, req->path);
	}

	simplify_path(path);

	DEBUG((DEBUG_INFO, "ls -l|path=%s\n", path));

	if (!file_existed(path)) {
		data->total = -1;
		DEBUG((DEBUG_INFO, "file not existed!\n"));
		return send_ll_resp(client->main_conn.sock, resp_packet,
			       resp, RESP_FILE_NTFD, 0);
	}

	if (is_file(path)) {
		data->total = 1;

		strncpy(data->entries[0].name, basename(path), FILE_NAME_MAX_LEN - 1);
		data->entries[0].type = FILE_TYPE_FILE;

		DEBUG((DEBUG_INFO, "list file successfully!\n"));

		return send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_OK, 0);
	} else if (is_dir(path)) {
		file_list = get_top_file_list(path);
		if (file_list == NULL) {
			data->total = -1;
			send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0);

			return -1;
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
				send_ll_resp(client->main_conn.sock, resp_packet,
					resp, RESP_OK, 1);
			} else {
				has_more = false;
				send_ll_resp(client->main_conn.sock, resp_packet,
					resp, RESP_OK, 0);
			}
		}
	}

	if (has_more)
		send_ll_resp(client->main_conn.sock, resp_packet,
			resp, RESP_OK, 0);

	DEBUG((DEBUG_INFO, "list file successfully!\n"));
	DEBUG((DEBUG_INFO, "handle ll req out\n"));

	return 0;
}

int handle_fwd_get_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	int ret;
	struct logged_in_user *user;
	struct get_req *req;
	struct get_resp *resp;
	struct client_sock_conn *conn = NULL;
	struct common_resp *com_resp;

	req = req_packet->obj;
	assert(req != NULL);

	resp = mp_malloc(g_mp, __func__, sizeof(struct get_resp));
	assert(resp != NULL);

	// check user info
	user = &req->user;
	if (check_user(user) == -1) {
		return send_get_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_CHECK_USER, 0);
	}

	// get or create peer session
	conn = get_peer_task_conn(user);
	
	if (conn == NULL) {
		DEBUG((DEBUG_INFO, "cannot get peer task conn!\n"));
		return send_get_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_GET_TASK_CONN, 0);
	}

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "get peer task conn|connect_id=%s\n",
				conn->connect_id));

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "send get req to peer|path=%s\n", req->path));

	// send get req packet to peer task conn
	ret = send_sftt_packet(conn->sock, req_packet);
	if (ret == -1) {
		DEBUG((DEBUG_INFO, "send get req to peer failed!\n"));
		ret = send_get_resp(client->main_conn.sock, resp_packet,
				resp, RESP_SEND_PEER_ERR, 0);
		goto done;
	}

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "send get req to peer done|path=%s\n",
				req->path));

	if (verbose_level > 0)
		DEBUG((DEBUG_INFO, "begin to recv file from peer\n"));

	do {
		// recv get resp packet
		ret = recv_sftt_packet(conn->sock, resp_packet);
		if (ret == -1) {
			DEBUG((DEBUG_INFO, "recv sftt packet failed!\n"));
			goto done;
		}
		resp = (struct get_resp *)resp_packet->obj;
		assert(resp != NULL);

		if (verbose_level > 0)
			DEBUG((DEBUG_INFO, "recv a packet\n"));

		if (verbose_level > 0)
			DEBUG((DEBUG_INFO, "send this packet to geter\n"));

		ret = send_get_resp(client->main_conn.sock, resp_packet,
			resp, RESP_OK, resp->next);

		if (resp->need_reply) {
			if (verbose_level > 0)
				DEBUG((DEBUG_INFO, "this packet need reply\n"));

			ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
			if (ret == -1) {
				DEBUG((DEBUG_INFO, "recv sftt packet failed!\n"));
				goto done;
			}

			if (verbose_level > 0)
				DEBUG((DEBUG_INFO, "received a reply common resp\n"));

			com_resp = (struct common_resp *)resp_packet->obj;
			assert(com_resp != NULL);

			if (verbose_level > 0)
				DEBUG((DEBUG_INFO, "send this common resp to getee\n"));

			ret = send_common_resp(conn->sock, resp_packet, com_resp, com_resp->status, 0);
		}

		if (verbose_level > 0)
			DEBUG((DEBUG_INFO, "have next?|next=%d\n", resp->next));

	} while (resp->next);

	DEBUG((DEBUG_INFO, "handle get fwd req done!\n"));
done:
	if (conn)
		put_peer_task_conn(conn);

	return ret;
}

#ifdef CONFIG_GET_OVERLAP
int handle_get_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct get_req *req;
	struct get_resp *resp;
	char path[FILE_NAME_MAX_LEN];
	int ret;

	DEBUG((DEBUG_INFO, "handle get req in ...\n"));

	resp = mp_malloc(g_mp, __func__, sizeof(struct get_resp));
	assert(resp != NULL);

	req = req_packet->obj;
	if (req->to_peer)
		return handle_fwd_get_req(client, req_packet, resp_packet);	

	strncpy(path, req->path, FILE_NAME_MAX_LEN);
	DEBUG((DEBUG_INFO, "get_req: session_id=%s|path=%s\n",
		req->session_id, req->path));

	if (!is_absolute_path(path)) {
		DEBUG((DEBUG_INFO, "path not absolute!\n"));
		return send_get_resp(client->main_conn.sock, resp_packet, resp,
				RESP_PATH_NOT_ABS, 0);
	}

	if (!file_existed(path)) {
		DEBUG((DEBUG_INFO, "file not existed!\n"));
		return send_get_resp(client->main_conn.sock, resp_packet, resp,
				RESP_FILE_NTFD, 0);
	}

	ret = send_files_by_get_resp(client->main_conn.sock, path, resp_packet,
			resp);

	DEBUG((DEBUG_INFO, "handle get req out\n"));

	return ret;
}
#else
int handle_get_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	return 0;
}
#endif

int handle_fwd_put_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	int ret;
	struct logged_in_user *user;
	struct peer_session *peer;
	struct put_req *req;
	struct put_resp *resp;
	struct common_resp *com_resp;
	struct client_sock_conn *conn = NULL;

	resp = mp_malloc(g_mp, __func__, sizeof(put_resp));
	if (resp == NULL) {
		DEBUG((DEBUG_INFO, "alloc put_resp failed!\n"));
		return -1;
	}

	req = req_packet->obj;
	// check user info
	user = &req->user;
	if (check_user(user) == -1) {
		return send_put_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_CHECK_USER, 0);
	}

	// get or create peer task conn
	conn = get_peer_task_conn(user);
	if (conn == NULL) {
		return send_put_resp(client->main_conn.sock, resp_packet,
			       resp, RESP_CNT_GET_TASK_CONN, 0);	
	}

#if 0
	// send put req packet to peer task conn
	ret = send_sftt_packet(conn->sock, req_packet);
	if (ret == -1) {
		DEBUG((DEBUG_INFO, "send ll req to peer failed!\n"));
		ret = send_put_resp(client->main_conn.sock, resp_packet,
				resp, RESP_SEND_PEER_ERR, 0);
		goto done;
	}
#endif

	do {
		// send put req packet to peer task conn
		ret = send_sftt_packet(conn->sock, req_packet);
		if (ret == -1) {
			DEBUG((DEBUG_ERROR, "send put req to peer failed!\n"));
			goto done;
		}

		req = (struct put_req *)req_packet->obj;
		if (req->need_reply) {
			ret = recv_sftt_packet(conn->sock, resp_packet);
			if (ret == -1) {
				DEBUG((DEBUG_ERROR, "recv sftt packet failed!\n"));
				goto done;
			}
			resp = (struct put_resp *)resp_packet->obj;
			assert(resp != NULL);
			ret = send_put_resp(client->main_conn.sock, resp_packet, resp, resp->status, 0);
		}

		if (!req->next)
			break;

		// recv put req packet
		ret = recv_sftt_packet(client->main_conn.sock, req_packet);
		if (ret == -1) {
			DEBUG((DEBUG_INFO, "recv sftt packet failed!\n"));
			goto done;
		}
	} while (req->next);

done:
	if (conn)
		put_peer_task_conn(conn);

	return 0;
}

int handle_put_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	int ret;
	struct put_req *req;

	DEBUG((DEBUG_INFO, "handle put req in ...\n"));

	req = req_packet->obj;
	if (req->to_peer)
		return handle_fwd_put_req(client, req_packet, resp_packet);

	ret = recv_files_by_put_req(client->main_conn.sock, req_packet);

	DEBUG((DEBUG_INFO, "handle put req out\n"));

	return ret;
}

int handle_directcmd_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct directcmd_req *req;
	struct directcmd_resp *resp;
	struct directcmd_resp_data *data;
	char cmd[CMD_MAX_LEN + FILE_NAME_MAX_LEN + 2];
	char temp_file[FILE_NAME_MAX_LEN];
	int ret;
	long total_len;
	FILE *fp;

	DEBUG((DEBUG_INFO, "handle directcmd req in ...\n"));
	resp = (struct directcmd_resp *)mp_malloc(g_mp, __func__,
			sizeof(struct directcmd_resp));
	assert(resp != NULL);
	data = &resp->data;

	req = req_packet->obj;
	assert(req != NULL);

	ret = create_temp_file(temp_file, "sfttd_directcmd_");
	if (ret == -1) {
		DEBUG((DEBUG_INFO, "cannot create temp file!\n"));
		return send_directcmd_resp(client->main_conn.sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0);
	}

	snprintf(cmd, sizeof(cmd), "%s > %s", req->cmd, temp_file);
	DEBUG((DEBUG_INFO, "%s\n", cmd));
	system(cmd);

	fp = fopen(temp_file, "rb");
	if (fp == NULL) {
		DEBUG((DEBUG_INFO, "open command result file failed!\n"));
		return send_directcmd_resp(client->main_conn.sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0);
	}

	total_len = 0;
	data->total_len = file_size(temp_file);
	do {
		ret = fread(data->content, 1, CMD_RET_BATCH_LEN - 1, fp);
		data->this_len = ret;
		total_len += data->this_len;
		
		if (total_len < data->total_len)
			ret = send_directcmd_resp(client->main_conn.sock, resp_packet,
					resp, RESP_OK, 1);
		else
			ret = send_directcmd_resp(client->main_conn.sock, resp_packet,
					resp, RESP_OK, 1);

		if (ret == -1) {
			printf("send mp stat response failed!\n");
		}
	} while (total_len < data->total_len);

	unlink(temp_file);

	return 0;
}

int handle_mp_stat_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct mp_stat_resp *resp;
	struct mp_stat_resp_data *data;
	struct mem_pool_stat stat;
	int ret;

	DEBUG((DEBUG_INFO, "handle mempool stat req in ...\n"));
	resp = (struct mp_stat_resp *)mp_malloc(g_mp, __func__,
			sizeof(struct mp_stat_resp));
	assert(resp != NULL);
	data = &resp->data;

	get_mp_stat(g_mp, &stat);
	data->total_size = stat.total_size;
	data->total_nodes = stat.total_nodes;
	data->using_nodes = stat.using_nodes;
	data->free_nodes = stat.free_nodes;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_MP_STAT_RESP;

	ret = send_sftt_packet(client->main_conn.sock, resp_packet);
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
			users[j].port = session->main_conn.port;
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
	struct who_resp_data *data;
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
	data = &resp->data;

	users = get_logged_in_users(&total);
	DEBUG((DEBUG_INFO, "There has %d user(s) logged in\n", total));
	if (users == NULL || total <= 0) {
		return send_who_resp(client->main_conn.sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0);
	}

	count = 0;
	do {
		num = (total - count) >= LOGGED_IN_USER_MAX_CNT ?
			LOGGED_IN_USER_MAX_CNT : (total - count);
		data->total = total;
		data->this_nr = num;
		for (i = 0; i < num; ++i) {
			bzero(&data->users[i], sizeof(struct logged_in_user));
			strncpy(data->users[i].session_id, users[i + count].session_id,
					SESSION_ID_LEN - 1);
			strncpy(data->users[i].ip, users[i + count].ip,
					IPV4_MAX_LEN - 1);
			strncpy(data->users[i].name, users[i + count].name,
					USER_NAME_MAX_LEN - 1);
			data->users[i].port = users[i + count].port;
		}

		count += num;
		ret = send_who_resp(client->main_conn.sock, resp_packet,
				resp, RESP_OK, count < total ? 1 : 0);
	} while (count < total);

	DEBUG((DEBUG_INFO, "handle who req out ...\n"));

	return ret;
}

int check_user(struct logged_in_user *user)
{
	int i, ret = -1;
	struct client_session *session;

	server->pm->ops->lock(server->pm);

	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		session = &server->sessions[i];
		if (client_connected(session) &&
			strcmp(session->session_id, user->session_id) == 0) {

			DEBUG((DEBUG_INFO, "session->session_id=%s|user->session_id=%s\n",
				session->session_id, user->session_id));

			strncpy(user->ip, session->ip, IPV4_MAX_LEN);
			ret = 0;
			goto done;
		}
	}

done:
	server->pm->ops->unlock(server->pm);

	return ret;
}

struct client_sock_conn *get_peer_task_conn(struct logged_in_user *user)
{
	struct peer_session *peer = NULL;
	struct peer_session *pos = NULL;
	struct client_session *target_session = NULL;
	struct client_sock_conn *conn = NULL;

	target_session = find_client_session_by_id(user->session_id);
	if (target_session == NULL) {
		DEBUG((DEBUG_INFO, "invalid session id: %s\n", user->session_id));
		return NULL;
	}

	target_session->tcs_lock->ops->lock(target_session->tcs_lock);
	list_for_each_entry(conn, &target_session->task_conns, list) {
		if (!conn->is_using) {
			conn->is_using = true;
			break;
		}
	}
	target_session->tcs_lock->ops->unlock(target_session->tcs_lock);	

	return conn;
}

void put_peer_task_conn(struct client_sock_conn *conn)
{
	conn->is_using = false;
}

int handle_write_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	int ret;
	struct write_req *req;
	struct logged_in_user *user;
	struct peer_session *peer;
	struct client_sock_conn *conn = NULL;

	req = req_packet->obj;
	user = &req->user;
	if (check_user(user) == -1) {
		printf("user not found!\n");
		return -1;
	}

	conn = get_peer_task_conn(user);
	if (conn == NULL) {
		printf("get or create peer session failed!\n");
		return -1;
	}

	ret = send_sftt_packet(conn->sock, req_packet);
	if (ret == -1) {
		printf("send write message to peer failed!\n");
		goto done;
	}

done:
	if (conn)
		put_peer_task_conn(conn);

	return 0;
}

struct client_session *find_client_session_by_id(char *session_id)
{
	int i;

	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		if (client_connected(&server->sessions[i]) &&
			strncmp(server->sessions[i].session_id, session_id,
				SESSION_ID_LEN) == 0) {
			return &server->sessions[i];
		}
	}

	return NULL;
}

int handle_append_conn_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct append_conn_req *req;
	struct client_session *real_session;
	struct append_conn_resp *resp;
	struct client_sock_conn *conn;
	int ret;

	DEBUG((DEBUG_INFO, "handle append_conn req in ...\n"));

	req = req_packet->obj;
	assert(req != NULL);

	DEBUG((DEBUG_INFO, "req->session_id=%s|req->type=%d\n",
			req->session_id, req->type));

	if (req->type != CONN_TYPE_TASK) {
		DEBUG((DEBUG_INFO, "invalid connect type: %d\n", req->type));
		return -1;
	}

	real_session = find_client_session_by_id(req->session_id);
	if (real_session == NULL) {
		DEBUG((DEBUG_INFO, "invalid session id: %s\n", req->session_id));
		return -1;
	}
	DEBUG((DEBUG_INFO, "find client session: session_id=%s\n",
			real_session->session_id));

	resp = mp_malloc(g_mp, "append_conn_resp", sizeof(struct append_conn_resp));
	if (resp == NULL) {
		DEBUG((DEBUG_INFO, "alloc append_conn_resp failed!\n"));
		return -1;
	}

	if (req->type == CONN_TYPE_TASK) {
		conn = mp_malloc(g_mp, "task_conn", sizeof(struct client_sock_conn));
		if (conn == NULL) {
			DEBUG((DEBUG_INFO, "alloc task_conn failed!\n"));
			return -1;
		}

		DEBUG((DEBUG_INFO, "conn=%p\n", conn));
		*conn = client->main_conn;
		DEBUG((DEBUG_INFO, "conn->connect_id=%s\n", conn->connect_id));
		conn->type = CONN_TYPE_TASK;
		conn->is_using = false;

		DEBUG((DEBUG_INFO, "conn->type=%d, conn->is_using=%d\n",
				conn->type, conn->is_using));
		list_add(&conn->list, &real_session->task_conns);

		DEBUG((DEBUG_INFO, "before copy connect_id\n"));
		strncpy(resp->data.connect_id, conn->connect_id, CONNECT_ID_LEN);
		DEBUG((DEBUG_INFO, "resp->data.connect_id=%s\n", resp->data.connect_id));
		DEBUG((DEBUG_INFO, "append a new conn: session_id=%s|connect_id=%s\n",
				real_session->session_id, conn->connect_id));

		ret = send_append_conn_resp(client->main_conn.sock, resp_packet, resp, RESP_OK, 0);
	} else {
		ret = send_append_conn_resp(client->main_conn.sock, resp_packet, resp,
				RESP_UNKNOWN_CONN_TYPE, 0);
	}

	DEBUG((DEBUG_INFO, "handle append_conn req out ...\n"));

	return ret;
}

void *handle_client_session(void *args)
{
	struct client_session *client = (struct client_session *)args;
	int sock = client->main_conn.sock;
	struct sftt_packet *resp;
	struct sftt_packet *req;
	int ret;

	DEBUG((DEBUG_INFO, "begin handle client session ...\n"));
	req = malloc_sftt_packet();
	if (!req) {
		printf("cannot allocate resources from memory pool!\n");
		return NULL;
	}

	resp = malloc_sftt_packet();
	if (resp == NULL) {
		goto exit;
	}

	signal(SIGTERM, child_process_exit);
	signal(SIGSEGV, child_process_exception_handler);

	add_log(LOG_INFO, "begin to communicate with client ...");
	while (1) {
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
		switch (req->type) {
		case PACKET_TYPE_VALIDATE_REQ:
			ret = validate_user_info(client, req, resp);
			if (ret == -1) {
				free_sftt_packet(&resp);
				goto exit;
			}
			break;
		case PACKET_TYPE_APPEND_CONN_REQ:
			handle_append_conn_req(client, req, resp);
			goto exit;
		case PACKET_TYPE_PWD_REQ:
			handle_pwd_req(client, req, resp);
			break;
		case PACKET_TYPE_CD_REQ:
			handle_cd_req(client, req, resp);
			break;
		case PACKET_TYPE_LL_REQ:
			handle_ll_req(client, req, resp);
			break;
		case PACKET_TYPE_PUT_REQ:
			handle_put_req(client, req, resp);
			break;
		case PACKET_TYPE_GET_REQ:
			handle_get_req(client, req, resp);
			break;
		case PACKET_TYPE_MP_STAT_REQ:
			handle_mp_stat_req(client, req, resp);
			break;
		case PACKET_TYPE_DIRECTCMD_REQ:
			handle_directcmd_req(client, req, resp);
			break;
		case PACKET_TYPE_WHO_REQ:
			handle_who_req(client, req, resp);
			break;
		case PACKET_TYPE_WRITE_REQ:
			handle_write_req(client, req, resp);
			break;
		default:
			printf("%s: cannot recognize packet type!\n", __func__);
			break;
		}
		//free_sftt_packet(&resp);
	}

exit:
	put_session(client);
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

struct client_session *get_new_session(void)
{
	int i;

	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		if (!client_connected(&server->sessions[i])) {

			init_client_session(&server->sessions[i]);
			set_client_active(&server->sessions[i]);

			return &server->sessions[i];
		}
	}

	return NULL;
}

void put_session(struct client_session *s)
{
	bzero(s, sizeof(struct client_session));
	set_client_disconnected(s);
}

void init_client_session(struct client_session *session)
{
	put_session(session);

	INIT_LIST_HEAD(&session->task_conns);

	session->tcs_lock = new(pthread_mutex);
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

	len = sizeof(struct sockaddr_in);
	while (1) {
		update_server(server);
		connect_fd = accept(server->main_sock, (struct sockaddr *)&addr_client, (socklen_t *)&len);
		if (connect_fd == -1) {
			usleep(100 * 1000);
			continue;
		}
		if ((session = get_new_session()) == NULL) {
			add_log(LOG_INFO, "exceed max connection!");
			close(connect_fd);
			continue;
		}
		add_log(LOG_INFO, "a client is connecting ...");

		bzero(session->ip, IPV4_MAX_LEN - 1);
		strncpy(session->ip, inet_ntoa(addr_client.sin_addr), IPV4_MAX_LEN - 1);
		session->main_conn.sock = connect_fd;
		session->main_conn.port = ntohs(addr_client.sin_port);
		session->main_conn.type = CONN_TYPE_CTRL;
		gen_connect_id(session->main_conn.connect_id, CONNECT_ID_LEN);

		DEBUG((DEBUG_INFO, "a client is connecting ...\n"));
		DEBUG((DEBUG_INFO, "ip=%s|port=%d\n", session->ip, session->main_conn.port));

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

	set_current_context("server");

	server = (struct sftt_server *)mp_malloc(g_mp, __func__, sizeof(struct sftt_server));
	assert(server != NULL);

	if (get_version_info(&server->ver) == -1) {
		printf("cannot get sfttd version info!\n");
		return -1;
	}

#if 0
	server->ver.major = 0;
	server->ver.minor = 1;
	server->ver.revision = 0;
#endif

	if (get_sftt_server_config(&(server->conf)) == -1) {
		printf("cannot get sfttd config!\n");
		return -1;
	}

	if (strlen(store_path)) {
		strcpy(server->conf.store_path, store_path);
	}

	sockfd = create_non_block_sock(&port);
	if (sockfd == -1) {
		printf("cannot create non-block socket!\n");
		return -1;
	}

	server->status = READY;
	server->main_sock = sockfd;
	server->main_port = port;
	server->last_update_ts = (uint64_t)time(NULL);

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

	return 0;
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
	show_version();
	printf("usage:\t" PROC_NAME " options\n"
		"\t" PROC_NAME " start [-v] [-d] [-s dir]\n"
		"\t" PROC_NAME " restart [-v] [-d] [-s dir]\n"
		"\t" PROC_NAME " stop [-v]\n"
		"\t" PROC_NAME " status [-v]\n");
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

	md5_str((unsigned char *)passwd, strlen(passwd), (unsigned char *)passwd_md5);

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
		md5_str((unsigned char *)value, strlen(value),
				(unsigned char *)passwd_md5);
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
	printf("name: %s, uid: %ld, home: %s, create time: %s, "
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
