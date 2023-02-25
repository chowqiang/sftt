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
#include "common.h"
#include "config.h"
#include "connect.h"
#include "context.h"
#include "debug.h"
#include "encrypt.h"
#include "file.h"
#include "file_trans.h"
#include "list.h"
#include "lock.h"
#include "log.h"
#include "mem_pool.h"
#include "mkdirp.h"
#include "net_trans.h"
#include "endpoint.h"
#include "thread_pool.h"
#include "response.h"
#include "rte_errno.h"
#include "rwlock.h"
#include "server.h"
#include "session.h"
#include "trans.h"
#include "version.h"
#include "user.h"
#include "utils.h"

#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)

struct batch_reserved reserveds[3] = {
	{"get_resp", sizeof(struct get_resp), 1000},
	{"put_req", sizeof(struct put_req), 1000},
	{"common_resp", sizeof(struct common_resp), 1000},
};

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

int create_non_block_sock(int port);

void put_sftt_server_stat(struct sftt_server_stat *sss);

struct sftt_server_stat *alloc_sftt_server_stat(void);

void sync_server_stat(void);

int check_user(struct logged_in_user *user);

void put_session(struct client_session *s);

struct client_session *find_client_session_by_id(char *session_id);

struct client_sock_conn *get_peer_task_conn_by_user(struct logged_in_user *user);

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
	pid_t   pid;
	unsigned char	buff[BUFFER_SIZE] = {'\0'};
	struct sftt_server_config  init_conf;
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

int init_sftt_server_stat(struct sftt_server *server)
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

	if (!file_existed(SFTT_SERVER_SHMKEY_FILE))
		create_new_file_with_parent(SFTT_SERVER_SHMKEY_FILE, DEFAULT_FILE_MODE);

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

static inline bool need_update_port(struct sftt_server *server)
{
	return get_ts() - server->last_update_ts > PORT_UPDATE_INTERVAL;
}

bool can_update_port(struct sftt_server *server)
{
	int i = 0;
	struct client_session *session;
	int conns_num = 0;

	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		session = &server->sessions[i];
		if (!client_connected(session))
			continue;

		conns_num = client_task_conns_num(session);
		if (conns_num < 1) {
			DEBUG((DEBUG_WARN, "there is no client task conn"
				"|num=%d|session_id=%s\n", conns_num,
				session->session_id));
			return false;
		}
	}

	return true;
}

static int validate_user_info(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	struct validate_req *req;
	struct validate_resp *resp;
	struct validate_resp_data *resp_data;
	struct user_base_info *user_base;
	struct user_auth_info *user_auth;
	int ret;

	DEBUG((DEBUG_WARN, "handle validate req in ...\n"));

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
		DBUG_RETURN(send_validate_resp(client->main_conn.sock, resp_packet,
				resp, RESP_UVS_BAD_VER, 0));
	}

	user_base = find_user_base_by_name(req->name);
	user_auth = find_user_auth_by_name(req->name);
	if (user_base == NULL) {
		DEBUG((DEBUG_INFO, "cannot find user!\n"));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
		DBUG_RETURN(send_validate_resp(client->main_conn.sock, resp_packet,
			       resp, RESP_UVS_NTFD, 0));
	} else if (strcmp(user_auth->passwd_md5, req->passwd_md5)) {
		DEBUG((DEBUG_INFO, "passwd not correct!\n"));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
		DBUG_RETURN(send_validate_resp(client->main_conn.sock, resp_packet,
				resp, RESP_UVS_INVALID, 0));
	} else if (!file_existed(user_base->home_dir)) {
		DEBUG((DEBUG_INFO, "cannot find user's home dir: %s\n", user_base->home_dir));
		DEBUG((DEBUG_INFO, "validate user info failed!\n"));
		DBUG_RETURN(send_validate_resp(client->main_conn.sock, resp_packet,
				resp, RESP_UVS_MISSHOME, 0));
	} else {
		client->status = ACTIVE;
		strncpy(client->pwd, user_base->home_dir, DIR_PATH_MAX_LEN - 1);
		memcpy(&client->user, user_base, sizeof(struct user_base_info));
		gen_session_id(client->session_id, SESSION_ID_LEN);

		resp->status = RESP_UVS_PASS;
		resp->flags = REQ_RESP_FLAG_NONE;

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
		DBUG_RETURN(-1);
	}

	DEBUG((DEBUG_INFO, "handle validate req out\n"));

	DBUG_RETURN(0);
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
	DBUG_ENTER(__func__);

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
		DBUG_RETURN(send_pwd_resp(client->main_conn.sock, resp_packet,
				resp, RESP_SESSION_INVALID, 0));
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
		DBUG_RETURN(-1);
	}

	DEBUG((DEBUG_INFO, "handle pwd req out\n"));

	DBUG_RETURN(0);
}

int handle_cd_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

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
		DBUG_RETURN(send_cd_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CANNOT_CD, 0));
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
		DBUG_RETURN(-1);
	}

	DEBUG((DEBUG_INFO, "handle cd req out\n"));

	DBUG_RETURN(0);
}

int send_ll_resp_once(struct client_session *client, struct ll_resp *resp_info,
	struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret;

	resp_packet->type = PACKET_TYPE_LL_RESP;
	resp_packet->obj = resp_info;

	ret = send_sftt_packet(client->main_conn.sock, resp_packet);
	if (ret == -1) {
		printf("send cd response failed!\n");
		DBUG_RETURN(-1);
	}

	DBUG_RETURN(0);
}

int handle_fwd_ll_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret;
	struct logged_in_user *user;
	struct ll_req *req;
	struct ll_resp *resp;
	struct client_sock_conn *conn = NULL;

	resp = mp_malloc(g_mp, __func__, sizeof(struct ll_resp));
	assert(resp != NULL);

	// check user info
	req = req_packet->obj;
	user = &req->user;
	if (check_user(user) == -1) {
		DBUG_RETURN(send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_CHECK_USER, 0));
	}

	// get or create peer task conn
	conn = get_peer_task_conn_by_user(user);
	if (conn == NULL) {
		DBUG_RETURN(send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_GET_PEER, 0));
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

	} while (!(resp->flags & REQ_RESP_FLAG_STOP));

done:
	if (conn)
		put_peer_task_conn(conn);

	DBUG_RETURN(ret);
}

int handle_ll_req(struct client_session *client, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

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
		DBUG_RETURN(handle_fwd_ll_req(client, req_packet, resp_packet));

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
		DBUG_RETURN(send_ll_resp(client->main_conn.sock, resp_packet,
			       resp, RESP_FILE_NTFD, 0));
	}

	if (is_file(path)) {
		data->total = 1;

		strncpy(data->entries[0].name, basename(path), FILE_NAME_MAX_LEN - 1);
		data->entries[0].type = FILE_TYPE_FILE;

		DEBUG((DEBUG_INFO, "list file successfully!\n"));

		DBUG_RETURN(send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_OK, 0));
	} else if (is_dir(path)) {
		file_list = get_top_file_list(path);
		if (file_list == NULL) {
			data->total = -1;
			send_ll_resp(client->main_conn.sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0);

			DBUG_RETURN(-1);
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
		dlist_destroy(file_list);
	}

	if (has_more)
		send_ll_resp(client->main_conn.sock, resp_packet,
			resp, RESP_OK, 0);

	DEBUG((DEBUG_INFO, "list file successfully!\n"));
	DEBUG((DEBUG_INFO, "handle ll req out\n"));

	DBUG_RETURN(0);
}

int handle_fwd_get_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret;
	struct logged_in_user *user;
	struct get_req *req;
	struct get_resp *resp;
	struct client_sock_conn *conn = NULL;
	struct common_resp *com_resp;
	bool stop = false;

	req = req_packet->obj;
	assert(req != NULL);

	resp = mp_malloc(g_mp, __func__, sizeof(struct get_resp));
	assert(resp != NULL);

	// check user info
	user = &req->user;
	if (check_user(user) == -1) {
		DBUG_RETURN(send_get_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_CHECK_USER, 0));
	}

	// get or create peer session
	conn = get_peer_task_conn_by_user(user);

	if (conn == NULL) {
		DEBUG((DEBUG_INFO, "cannot get peer task conn!\n"));
		DBUG_RETURN(send_get_resp(client->main_conn.sock, resp_packet,
				resp, RESP_PEER_BUSYING, 0));
	}

	DEBUG((DEBUG_INFO, "get peer task conn|connect_id=%s\n",
				conn->connect_id));

	DEBUG((DEBUG_INFO, "send get req to peer|path=%s\n", req->path));

	// send get req packet to peer task conn
	ret = send_sftt_packet(conn->sock, req_packet);
	if (ret == -1) {
		DEBUG((DEBUG_WARN, "send get req to peer failed!\n"));
		ret = send_get_resp(client->main_conn.sock, resp_packet,
				resp, RESP_SEND_PEER_ERR, 0);
		goto done;
	}

	DEBUG((DEBUG_INFO, "send get req to peer done|path=%s\n",
				req->path));

	DEBUG((DEBUG_INFO, "begin to recv file from peer\n"));

	do {
		// recv get resp packet from peer
		ret = recv_sftt_packet(conn->sock, resp_packet);
		if (ret == -1) {
			DEBUG((DEBUG_ERROR, "recv sftt packet failed!\n"));
			goto done;
		}
		resp = (struct get_resp *)resp_packet->obj;
		assert(resp != NULL);

		DEBUG((DEBUG_INFO, "recv a packet\n"));

		DEBUG((DEBUG_INFO, "send this packet to geter\n"));

		ret = send_get_resp(client->main_conn.sock, resp_packet,
			resp, RESP_OK, resp->flags);

		if (resp->need_reply) {
			DEBUG((DEBUG_INFO, "this packet need reply\n"));

			ret = recv_sftt_packet(client->main_conn.sock, resp_packet);
			if (ret == -1) {
				DEBUG((DEBUG_ERROR, "recv sftt packet failed!\n"));
				goto done;
			}

			DEBUG((DEBUG_INFO, "received a reply common resp\n"));

			com_resp = (struct common_resp *)resp_packet->obj;
			assert(com_resp != NULL);

			DEBUG((DEBUG_INFO, "send this common resp to getee\n"));

			ret = send_common_resp(conn->sock, resp_packet, com_resp, com_resp->status, 0);
			if (ret == -1) {
				DEBUG((DEBUG_ERROR, "fwd common resp failed!\n"));
				break;
			}
		}

		DEBUG((DEBUG_INFO, "need stop?|stop=%d\n", (resp->flags & REQ_RESP_FLAG_STOP)));
	} while (!(resp->flags & REQ_RESP_FLAG_STOP));

	DEBUG((DEBUG_INFO, "handle get fwd req done!\n"));
done:
	if (conn)
		put_peer_task_conn(conn);

	DBUG_RETURN(ret);
}

#ifdef CONFIG_GET_OVERLAP
int handle_get_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	struct get_req *req;
	struct get_resp *resp;
	char path[FILE_NAME_MAX_LEN];
	int ret;

	DEBUG((DEBUG_INFO, "handle get req in ...\n"));

	resp = mp_malloc(g_mp, __func__, sizeof(struct get_resp));
	assert(resp != NULL);

	req = req_packet->obj;
	if (req->to_peer)
		DBUG_RETURN(handle_fwd_get_req(client, req_packet, resp_packet));

	strncpy(path, req->path, FILE_NAME_MAX_LEN);
	DEBUG((DEBUG_INFO, "get_req: session_id=%s|path=%s\n",
		req->session_id, req->path));

	if (!is_absolute_path(path)) {
		DEBUG((DEBUG_INFO, "path not absolute!\n"));
		DBUG_RETURN(send_get_resp(client->main_conn.sock, resp_packet, resp,
				RESP_PATH_NOT_ABS, 0));
	}

	if (!file_existed(path)) {
		DEBUG((DEBUG_INFO, "file not existed!\n"));
		DBUG_RETURN(send_get_resp(client->main_conn.sock, resp_packet, resp,
				RESP_FILE_NTFD, 0));
	}

	ret = send_files_by_get_resp(client->main_conn.sock, path, req_packet,
			resp_packet, resp);

	DEBUG((DEBUG_INFO, "handle get req out\n"));

	DBUG_RETURN(ret);
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
	DBUG_ENTER(__func__);

	int ret;
	struct logged_in_user *user;
	struct put_req *req;
	struct put_resp *resp;
	struct common_resp *com_resp;
	struct client_sock_conn *conn = NULL;

	resp = mp_malloc(g_mp, __func__, sizeof(put_resp));
	if (resp == NULL) {
		DEBUG((DEBUG_INFO, "alloc put_resp failed!\n"));
		DBUG_RETURN(-1);
	}

	req = req_packet->obj;
	// check user info
	user = &req->user;
	if (check_user(user) == -1) {
		DBUG_RETURN(send_put_resp(client->main_conn.sock, resp_packet,
				resp, RESP_CNT_CHECK_USER, 0));
	}

	// get or create peer task conn
	conn = get_peer_task_conn_by_user(user);
	if (conn == NULL) {
		DBUG_RETURN(send_put_resp(client->main_conn.sock, resp_packet,
			       resp, RESP_PEER_BUSYING, 0));
	}

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

		if (req->flags & REQ_RESP_FLAG_STOP)
			break;

		// recv put req packet
		ret = recv_sftt_packet(client->main_conn.sock, req_packet);
		if (ret == -1) {
			DEBUG((DEBUG_WARN, "recv sftt packet failed!\n"));
			goto done;
		}
	} while (!(req->flags & REQ_RESP_FLAG_STOP));

done:
	if (conn)
		put_peer_task_conn(conn);

	DBUG_RETURN(0);
}

int handle_put_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret;
	struct put_req *req;

	DEBUG((DEBUG_INFO, "handle put req in ...\n"));

	req = req_packet->obj;
	if (req->to_peer)
		DBUG_RETURN(handle_fwd_put_req(client, req_packet, resp_packet));

	ret = recv_files_from_put_req(client->main_conn.sock, req_packet, resp_packet);

	DEBUG((DEBUG_INFO, "handle put req out\n"));

	DBUG_RETURN(ret);
}

int handle_directcmd_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

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
		DBUG_RETURN(send_directcmd_resp(client->main_conn.sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0));
	}

	snprintf(cmd, sizeof(cmd), "%s > %s", req->cmd, temp_file);
	DEBUG((DEBUG_INFO, "%s\n", cmd));
	system(cmd);

	fp = fopen(temp_file, "rb");
	if (fp == NULL) {
		DEBUG((DEBUG_INFO, "open command result file failed!\n"));
		DBUG_RETURN(send_directcmd_resp(client->main_conn.sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0));
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

	DBUG_RETURN(0);
}

int handle_mp_stat_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

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

	DBUG_RETURN(ret);
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
	DBUG_ENTER(__func__);

	struct who_resp *resp;
	struct who_resp_data *data;
	struct logged_in_user *users;
	int total, num, count;
	int ret, i = 0;

	DEBUG((DEBUG_WARN, "handle who req in ...\n"));
	resp = (struct who_resp *)mp_malloc(g_mp, __func__,
			sizeof(struct who_resp));

	if (resp == NULL) {
		DEBUG((DEBUG_INFO, "cannot alloc memory for resp!\n"));
		DBUG_RETURN(-1);
	}
	data = &resp->data;

	users = get_logged_in_users(&total);
	DEBUG((DEBUG_INFO, "There has %d user(s) logged in\n", total));
	if (users == NULL || total <= 0) {
		DBUG_RETURN(send_who_resp(client->main_conn.sock, resp_packet,
				resp, RESP_INTERNAL_ERR, 0));
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

	DBUG_RETURN(ret);
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

struct client_sock_conn *get_peer_task_conn_by_user(struct logged_in_user *user)
{
	struct client_session *target_session = NULL;
	struct client_sock_conn *conn = NULL;
	bool found = false;

	target_session = find_client_session_by_id(user->session_id);
	if (target_session == NULL) {
		DEBUG((DEBUG_INFO, "invalid session id: %s\n", user->session_id));
		return NULL;
	}

	return get_peer_task_conn_by_session(target_session);
}


int handle_write_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret;
	struct write_req *req;
	struct logged_in_user *user;
	struct client_sock_conn *conn = NULL;

	req = req_packet->obj;
	user = &req->user;
	if (check_user(user) == -1) {
		printf("user not found!\n");
		DBUG_RETURN(-1);
	}

	conn = get_peer_task_conn_by_user(user);
	if (conn == NULL) {
		printf("get or create peer session failed!\n");
		DBUG_RETURN(-1);
	}

	ret = send_sftt_packet(conn->sock, req_packet);
	if (ret == -1) {
		printf("send write message to peer failed!\n");
		goto done;
	}

done:
	if (conn)
		put_peer_task_conn(conn);

	DBUG_RETURN(0);
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
	DBUG_ENTER(__func__);

	struct append_conn_req *req;
	struct client_session *real_session;
	struct append_conn_resp *resp;
	struct client_sock_conn *conn;
	int ret;

	DEBUG((DEBUG_WARN, "handle append_conn req in ...\n"));

	req = req_packet->obj;
	assert(req != NULL);

	DEBUG((DEBUG_INFO, "req->session_id=%s|req->type=%d\n",
			req->session_id, req->type));

	if (req->type != CONN_TYPE_TASK) {
		DEBUG((DEBUG_INFO, "invalid connect type: %d\n", req->type));
		DBUG_RETURN(-1);
	}

	real_session = find_client_session_by_id(req->session_id);
	if (real_session == NULL) {
		DEBUG((DEBUG_INFO, "invalid session id: %s\n", req->session_id));
		DBUG_RETURN(-1);
	}
	DEBUG((DEBUG_INFO, "find client session: session_id=%s\n",
			real_session->session_id));

	resp = mp_malloc(g_mp, "append_conn_resp", sizeof(struct append_conn_resp));
	if (resp == NULL) {
		DEBUG((DEBUG_INFO, "alloc append_conn_resp failed!\n"));
		DBUG_RETURN(-1);
	}

	if (req->type == CONN_TYPE_TASK) {
		conn = mp_malloc(g_mp, "task_conn", sizeof(struct client_sock_conn));
		if (conn == NULL) {
			DEBUG((DEBUG_INFO, "alloc task_conn failed!\n"));
			DBUG_RETURN(-1);
		}

		DEBUG((DEBUG_INFO, "conn=%p\n", conn));
		*conn = client->main_conn;
		DEBUG((DEBUG_INFO, "conn->connect_id=%s\n", conn->connect_id));
		conn->type = CONN_TYPE_TASK;
		atomic16_set(&conn->is_using, 0);

		DEBUG((DEBUG_INFO, "append task conn|conn->type=%d|conn->is_using=%d\n",
				conn->type, atomic16_read(&conn->is_using)));
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

	DBUG_RETURN(ret);
}

struct client_sock_conn *find_connect_by_id(struct client_session *session,
	char *connect_id)
{
	struct client_sock_conn *conn = NULL;

	if (strcmp(session->main_conn.connect_id, connect_id) == 0)
		return &session->main_conn;

	list_for_each_entry(conn, &session->task_conns, list) {
		if (strcmp(conn->connect_id, connect_id) == 0)
			return conn;
	}

	return NULL;
}

int handle_reconnect_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	struct client_session *target_session = NULL;
	struct client_sock_conn *conn = NULL;
	struct reconnect_req *req;

	req = req_packet->obj;
	target_session = find_client_session_by_id(req->session_id);
	if (target_session == NULL) {
		DEBUG((DEBUG_ERROR, "invalid session id: %s\n", req->session_id));
		return -1;
	}

	conn = find_connect_by_id(target_session, req->connect_id);
	if (conn == NULL) {
		DEBUG((DEBUG_ERROR, "invalid connect id: %s\n", req->connect_id));
		return -1;
	}

	conn->sock = client->main_conn.sock;
	clear_conn_updating(conn);

	return 0;
}

int install_child_sigactions(void)
{
	struct sigaction child_exit;
	struct sigaction child_exception;
	int ret;

	child_exit.sa_handler = child_process_exit;
	ret = sigaction(SIGINT, &child_exit, NULL);
	if (ret == -1)
		return ret;

	child_exception.sa_handler = child_process_exception_handler;
	return sigaction(SIGSEGV, &child_exception, NULL);
}

int handle_client_session(void *args)
{
	DBUG_ENTER(__func__);

	struct client_session *client = (struct client_session *)args;
	struct sftt_packet *resp = NULL;
	struct sftt_packet *req = NULL;
	int ret = -1;
	bool need_close = true;

	DEBUG((DEBUG_INFO, "begin handle client session ...\n"));
	req = malloc_sftt_packet();
	if (req == NULL) {
		DEBUG((DEBUG_ERROR, "alloc sftt packet failed!\n"));
		goto exit;
	}

	resp = malloc_sftt_packet();
	if (resp == NULL) {
		DEBUG((DEBUG_ERROR, "alloc sftt packet failed!\n"));
		goto exit;
	}

	if (install_child_sigactions() == -1) {
		DEBUG((DEBUG_ERROR, "set child sigactions failed!\n"));
	}

	if (make_socket_non_blocking(client->main_conn.sock) == -1) {
		DEBUG((DEBUG_ERROR, "set sock non blocking failed!\n"));
	}

	add_log(LOG_INFO, "begin to communicate with client ...");
	while (1) {
		if (rwlock_read_trylock(&server->update_lock)) {
			sleep(1);
		}
		while (is_conn_updating(&client->main_conn)) {
			sleep(1);
		}

		get_sock_conn(&client->main_conn);
		ret = recv_sftt_packet(client->main_conn.sock, req);
		add_log(LOG_INFO, "%s: recv return|ret=%d", __func__, ret);
		if (ret == 0) {
			add_log(LOG_INFO, "client disconnected, child process is exiting ...");
			DEBUG((DEBUG_INFO, "a client is disconnected\n"));
			goto exit;
		}

		if (ret == -1) {
			if (rte_errno == EAGAIN || rte_errno == EWOULDBLOCK) {
				usleep(100 * 1000);
				continue;
			} else {
				add_log(LOG_ERROR, "recv from client failed, child process is exiting ...");
				DEBUG((DEBUG_ERROR, "recv from client failed, child process is exiting ...\n"));
				goto exit;
			}
		}

		switch (req->type) {
		case PACKET_TYPE_VALIDATE_REQ:
			ret = validate_user_info(client, req, resp);
			if (ret == -1)
				goto exit;
			break;
		case PACKET_TYPE_APPEND_CONN_REQ:
			handle_append_conn_req(client, req, resp);
			need_close = false;
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
		case PACKET_TYPE_RECONNECT_REQ:
			handle_reconnect_req(client, req, resp);
			goto exit;
		default:
			DEBUG((DEBUG_WARN, "cannot recognize packet type|type=%d\n", req->type));
			break;
		}
		put_sock_conn(&client->main_conn);
	}

exit:
	if (req)
		free_sftt_packet(&req);

	if (resp)
		free_sftt_packet(&resp);

	put_sock_conn(&client->main_conn);
	put_session(client);

	if (need_close)
		close(client->main_conn.sock);

	DBUG_RETURN(ret);
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

static int create_state_file(struct sftt_server *server)
{
	int ret = 0;

	if (server->state_file)
		ret = create_new_file(server->state_file, DEFAULT_FILE_MODE);

	return ret;
}

void respond_channel_info(int sock)
{
	struct sftt_packet *resp_packet;
	struct channel_info_resp *resp;
	int ret;

	if ((resp_packet = malloc_sftt_packet()) == NULL) {
		DEBUG((DEBUG_ERROR, "alloc sftt packet failed!\n"));
		return;
	}

	if ((resp = mp_malloc(g_mp, __func__, sizeof(struct channel_info_resp))) == NULL) {
		DEBUG((DEBUG_ERROR, "alloc channel info resp failed!\n"));
		return;
	}

	resp->data.main_port = server->main_port;
	resp->data.second_port = server->second_port;

	ret = send_channel_info_resp(sock, resp_packet, resp, RESP_OK, REQ_RESP_FLAG_NONE);
	if (ret == -1) {
		DEBUG((DEBUG_ERROR, "send channel info resp failed!\n"));
	}
}

void main_channel_loop(void)
{
	int connect_fd = 0;
	pid_t pid = 0;
	int idx = 0;
	int ret;
	pthread_t child;
	struct client_session *session;
	struct sockaddr_in addr_client;
	int len;

	server->status = SERVERING;

	create_state_file(server);

	len = sizeof(struct sockaddr_in);
	while (1) {
		rwlock_read_lock(&server->update_lock);
		connect_fd = accept(server->main_sock, (struct sockaddr *)&addr_client, (socklen_t *)&len);
		if (connect_fd == -1) {
			usleep(100 * 1000);
			continue;
		}

		respond_channel_info(connect_fd);
		close(connect_fd);
		rwlock_read_unlock(&server->update_lock);
	}
}

int create_non_block_sock(int port)
{
	int	sockfd;
	struct sockaddr_in serveraddr;

	add_log(LOG_INFO, "port is %d", port);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("create socket filed");
		return -1;
	}

	if (make_socket_non_blocking(sockfd) == -1) {
		perror("set sockfd to non-block failed");
		return -1;
	}

	memset(&serveraddr, 0 ,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		perror("bind socket error");
		DEBUG((DEBUG_ERROR, "bind socket error|port=%d\n", port));
		return -1;
	}

	if (listen(sockfd, 10) == -1){
		perror("listen socket error");
		return -1;
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

	ret = launch_thread_in_pool(server->thread_pool, THREAD_INDEX_ANY,
			logger_daemon, &logger_ctx);
	if (ret)
		return -1;

	return 0;
}

int init_sftt_server_thread_pool(struct sftt_server *server)
{
	server->thread_pool = create_thread_pool(16);
	if (server->thread_pool == NULL)
		return -1;

	return 0;
}

int init_sftt_server(char *store_path, char *state_file)
{
	int port = 0;
	int sockfd;
	int ret;

	set_current_context("server");

	server->state_file = state_file;

	if (get_version_info(&server->ver) == -1) {
		printf("cannot get sfttd version info!\n");
		return -1;
	}

	if (get_sftt_server_config(&(server->conf)) == -1) {
		printf("cannot get sfttd config!\n");
		return -1;
	}

	if (strlen(store_path)) {
		strcpy(server->conf.store_path, store_path);
	}

#if CONFIG_USE_RANDOM_PORT
	port = get_pseudo_random_port();
#else
	port = get_default_port();
#endif
	if ((sockfd = create_non_block_sock(port)) == -1) {
		DEBUG((DEBUG_ERROR, "cannot create main socket!\n"));
		return -1;
	}
	server->main_sock = sockfd;
	server->main_port = port;

	port = get_real_random_port();
	if (port == server->main_port) {
		DEBUG((DEBUG_ERROR, "get second port failed!\n"));
		return -1;
	}
	if ((sockfd = create_non_block_sock(port)) == -1) {
		DEBUG((DEBUG_ERROR, "cannot create second socket!\n"));
		return -1;
	}
	server->second_sock = sockfd;
	server->second_port = port;
	server->last_update_ts = get_ts();

	if (init_sftt_server_thread_pool(server) == -1) {
		printf("cannot create thread pool!\n");
		return -1;
	}

	if (start_sftt_log_server(server) == -1) {
		printf("cannot start log server!\n");
		return -1;
	}

	if (init_sftt_server_stat(server)) {
		printf(PROC_NAME " start failed! Because cannot init "
			PROC_NAME " server info.\n");
		return -1;
	}

	server->pm = new(pthread_mutex);

	server->status = READY;

	return 0;
}

int install_server_sigactions(void)
{
	struct sigaction server_exit;

	server_exit.sa_handler = sftt_server_exit;

	sigaction(SIGINT, &server_exit, NULL);

	return sigaction(SIGTERM, &server_exit, NULL);
}

int notify_client_after_updating(void)
{
	DBUG_ENTER(__func__);

	int i = 0;
	struct sftt_packet *req_packet;
	struct client_sock_conn *conn;
	struct port_update_req *req;
	int tmp, ret = 0;
	struct client_session *session;

	req_packet = malloc_sftt_packet();
	if (req_packet == NULL) {
		DEBUG((DEBUG_ERROR, "alloc update port req packet failed!\n"));
		DBUG_RETURN(-1);
	}

	req = mp_malloc(g_mp, __func__, sizeof(struct port_update_req));
	if (req == NULL) {
		DEBUG((DEBUG_ERROR, "alloc update port req failed!\n"));
		free_sftt_packet(&req_packet);
		DBUG_RETURN(-1);
	}

	req->second_port = server->second_port;
	req_packet->obj = req;
	req_packet->type = PACKET_TYPE_PORT_UPDATE_REQ;

	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		session = &server->sessions[i];
		if (!client_connected(session))
			continue;

		list_for_each_entry(conn, &session->task_conns, list) {
			DEBUG((DEBUG_WARN, "send new port to client|port=%d|session_id=%s|"
					"connect_id=%s\n",req->second_port,
					session->session_id, conn->connect_id));
			tmp = send_sftt_packet(conn->sock, req_packet);
			if (tmp == -1) {
				DEBUG((DEBUG_ERROR, "send port update req failed!\n"));
				ret = tmp;
			}
			//close(conn->sock);
		}
		close(session->main_conn.sock);
	}

	free_sftt_packet(&req_packet);
	mp_free(g_mp, req);

	DBUG_RETURN(ret);
}

void set_all_conns_updating(void)
{
	int i = 0;
	struct client_session *session;
	int conns_num = 0;
	struct client_sock_conn *conn;

	for (i = 0; i < MAX_CLIENT_NUM; ++i) {
		session = &server->sessions[i];
		if (!client_connected(session))
			continue;

		list_for_each_entry(conn, &session->task_conns, list) {
			set_conn_updating(conn);
		}
	}
}

/**
 * Suppose there are some kinds of threads
 * S1: server main thread
 * S2: derived from S1, used to accept connects from the main port
 *     and reply the second port
 * S3: derived from S1, used to accept connects from the second port
 * S4: derived from S3, used to handle requests from the clinet
 * S5: derived from S3, used to send requests to client
 * S6: derived from S3, same to S5
 * S7: derived from S3, same to S5
 * U:  derived from S1, used to update the second port
 *
 * When need to update port:
 * U:  get write lock on server->update_lock, use channels of S5, S6, S7 to send
 *     the new second port
 * S2: get read lock on server->update_lock, wait for the new second port if
 *     blocked
 * S3: get read lock on server->update_lock, wait for the new socket of the
 *     new second port if blocked
 * S4: try get read lock, sleep X seconds for U to set conn->is_updating as one,
 *     and period to check the conn->is_updating until it equals to zero
 * S5: same to S4
 * S6: same to S4
 * S7: same to S4
 */
int port_update_loop(void *arg)
{
	struct sftt_server *server = arg;
	int sock = 0;
	int port;
	char buf[128];

	for (;;) {
		if (!need_update_port(server)) {
			sleep(1);
			continue;
		}

		if (!can_update_port(server)) {
			sleep(1);
			continue;
		}

		if (rwlock_write_trylock(&server->update_lock)) {
			sleep(1);
			continue;
		}

		port = get_real_random_port();
		if (server->main_port == port ||
			server->second_port == port) {
			DEBUG((DEBUG_WARN, "new port equal to old port|main_port=%d|second_port=%d"
					"|new_port=%d\n", server->main_port, server->second_port,
					port));
			rwlock_write_unlock(&server->update_lock);
			sleep(1);
			continue;
		}

		sock = create_non_block_sock(port);
		if (sock == -1) {
			DEBUG((DEBUG_ERROR, "create non block sock failed when update port!\n"));
			rwlock_write_unlock(&server->update_lock);
			sleep(1);
			continue;
		}

		set_all_conns_updating();

		close(server->second_sock);
		server->second_port = port;
		server->second_sock = sock;

		notify_client_after_updating();

		sprintf(buf, "update second port and second sock done!\n"
			"sock(%d -> %d), port(%d -> %d)", server->second_sock,
			sock, server->second_port, port);
		add_log(LOG_INFO, buf);

		server->last_update_ts = get_ts();
		sync_server_stat();
		rwlock_write_unlock(&server->update_lock);
	}

	return -1;
}

int start_port_update_thread(void)
{
	int ret;

	ret = launch_thread_in_pool(server->thread_pool, THREAD_INDEX_ANY,
			port_update_loop, server);

	return ret;
}

int second_channel_loop(void *arg)
{
	int connect_fd = 0;
	pid_t pid = 0;
	int idx = 0;
	int ret;
	pthread_t child;
	struct client_session *session;
	struct sockaddr_in addr_client;
	int len;

	init_sessions();

	len = sizeof(struct sockaddr_in);
	while (1) {
		rwlock_read_lock(&server->update_lock);
		connect_fd = accept(server->second_sock, (struct sockaddr *)&addr_client, (socklen_t *)&len);
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

		DEBUG((DEBUG_WARN, "a client is connecting ...|second_sock=%d|second_port=%d|"
				   "ip=%s|port=%d\n", server->second_sock, server->second_port,
				   session->ip, session->main_conn.port));

		ret = launch_thread_in_pool(server->thread_pool, THREAD_INDEX_ANY,
				handle_client_session, session);
		if (ret) {
			add_log(LOG_INFO, "create thread failed!");
			session->status = DISCONNECTED;
		}
		rwlock_read_unlock(&server->update_lock);
	}

}

int start_second_channel(void)
{
	int ret;

	ret = launch_thread_in_pool(server->thread_pool, THREAD_INDEX_ANY,
			second_channel_loop, server);

	return ret;
}

int sftt_server_start(struct sftt_server *ser, char *store_path, bool background, char *state_file)
{
	server = ser;

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

	if (init_sftt_server(store_path, state_file) == -1) {
		printf(PROC_NAME " create server failed!\n");
		exit(-1);
	}

#ifdef CONFIG_UPDATE_CHANNEL
	if (start_port_update_thread() == -1) {
		DEBUG((DEBUG_ERROR, "start port updating thread failed!\n"));
	}
#endif

	add_log(LOG_INFO, PROC_NAME " is going to start in the background ...");

	install_server_sigactions();

	if (start_second_channel() == -1) {
		DEBUG((DEBUG_ERROR, "start second channel failed!\n"));
		exit(-1);
	}

	main_channel_loop();

	return 0;
}

int sftt_server_restart(struct sftt_server *ser, char *store_path, bool background, char *state_file)
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
	sftt_server_start(ser, store_path, background, state_file);

	add_log(LOG_INFO, PROC_NAME " is going to restart ...");

	return 0;
}

/*
 * we should wait all children exit before sftt server exits.
 * Todo: fix this bug.
 */
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

	logger_exit();

	printf(PROC_NAME " pid is: %d\n", sss->main_pid);
	printf(PROC_NAME " is going to stop ...\n");
	kill(sss->main_pid, SIGINT);
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

void close_channels(void)
{
	DEBUG((DEBUG_WARN, "close channels|main_sock=%d|second_sock=%d\n",
				server->main_sock, server->second_sock));
	close(server->main_sock);
	close(server->second_sock);
}

void sftt_server_exit(int sig)
{
	DEBUG((DEBUG_WARN, "is exit ...!\n"));

	notify_all_child_to_exit();

	close_channels();

	free_sftt_server_stat();

	logger_exit();

	exit(-1);
}

void server_usage_help(int exitcode)
{
	show_version();
	printf("usage:\t" PROC_NAME " options\n"
		"\t" PROC_NAME " start [-v] [-d] [-s state_file]\n"
		"\t" PROC_NAME " restart [-v] [-d] [-s state_file]\n"
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
