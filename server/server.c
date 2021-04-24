#include <assert.h>
#include <stdio.h>
#include <sys/socket.h>  
#include <signal.h>
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
#include "base.h"
#include "config.h"
#include "debug.h"
#include "encrypt.h"
#include "log.h"
#include "mem_pool.h"
#include "net_trans.h"
#include "random_port.h"
#include "req_rsp.h"
#include "server.h"
#include "session.h"
#include "version.h"
#include "user.h"
#include "utils.h"

#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)  

extern struct mem_pool *g_mp;

struct sftt_server *server;

int create_non_block_sock(int *pport);

static void sftt_server_exit(int sig);

void put_sftt_server_stat(struct sftt_server_stat *sss);

struct sftt_server_stat *alloc_sftt_server_stat(void);

void sync_server_stat(void);

//static struct child_info *get_child_process_info(pid_t pid);
//static void register_child_process_info(int index, pid_t pid);

void server_init_func(struct sftt_server_config *server_config){
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

int server_consult_block_size(int connect_fd,char *buff,int server_block_size){
	int trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
	if (trans_len <= 0 ) {
		printf("consult block size recv failed!\n");
		exit(0);
	}

	char *mybuff = sftt_decrypt_func(buff,trans_len);
	int client_block_size = atoi(buff);
	int min_block_size = client_block_size < server_block_size ? client_block_size : server_block_size;

	sprintf(buff,"%d",min_block_size);
	int size = strlen(buff);
	sftt_encrypt_func(buff,size);
	send(connect_fd,buff,BUFFER_SIZE,0);

	return min_block_size;

}

void server_file_resv(int connect_fd, int consulted_block_size, struct sftt_server_config init_conf){
	int trans_len;
	struct sftt_packet *sp = malloc_sftt_packet(consulted_block_size);
	int connected = 1;
	while (connected){
		FILE * fd;
		int i = 0 ;
		int j = 0 ; 
		char *data_buff = (char *)malloc(consulted_block_size * sizeof(char));
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

void server_transport_data_to_file(FILE * fd, struct sftt_packet * sp ){
	int write_len=fwrite(sp->content, 1, sp->data_len, fd);
	add_log(LOG_INFO, "write len is %d", write_len);
}


FILE * server_creat_file(struct sftt_packet *sp, struct sftt_server_config init_conf, char * data_buff){
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

void  is_exit(char * filepath){
	char * tmp_path = (char * ) malloc (strlen(filepath)*sizeof(char));
	printf("%s ======file_path\n",filepath);
	memset(tmp_path,'\0',strlen(filepath));
	strcpy(tmp_path,filepath);
	printf("tmp_path == %s ",tmp_path);
	int str_len = strlen(filepath);
	int i ;
	for (i = 0; i <= str_len; i ++ ) {
		if (tmp_path[i] == '/'){ 
			tmp_path[i+1] = '\0';
			if (access(tmp_path, F_OK) == -1) {
				int status = mkdir(tmp_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				if (status == -1) {
                    printf("%s\n",tmp_path);
                }
			}else {
				printf("%s\n", tmp_path);
			}
			memset(tmp_path,'\0',str_len);
            strcpy(tmp_path,filepath);
			printf("%s=====",tmp_path);
            continue;
		}
	}
}

int main_old(){
//	int	socket_fd = sftt_server();
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
		if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){  
		printf("connect filed");	
		continue;
		}
		pid = fork();
		if ( pid == 0 ){
			int consulted_block_size;
			consulted_block_size = server_consult_block_size(connect_fd,buff,init_conf.block_size);
			printf("consulted_block_size : %d\n",consulted_block_size);
			server_file_resv(connect_fd, consulted_block_size,init_conf );
		} else if (pid < 0 ){
			printf("fork failed!\n");
		} else {
			wait(NULL);
		}
		
	}
	close(socket_fd);
}

void sighandler(int signum) {
   add_log(LOG_INFO, "Caught signal %d, coming out ...", signum);
}

bool init_sftt_server_stat(pid_t log_pid) {
	struct sftt_server_stat *sss = alloc_sftt_server_stat();
	assert(sss != NULL);

	sss->main_pid = getpid();
	sss->log_pid = log_pid;
	memcpy(&sss->conf, &server->conf, sizeof(struct sftt_server_config));

	put_sftt_server_stat(sss);
}

int get_sftt_server_shmid(int create_flag) {
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

void *get_sftt_server_shmaddr(int create_flag) {
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

struct sftt_server_stat *get_sftt_server_stat(void) {
	return (struct sftt_server_stat *)get_sftt_server_shmaddr(0);
}

void put_sftt_server_stat(struct sftt_server_stat *sss)
{
	shmdt(sss);
}

bool free_sftt_server_stat(void) {
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

bool sftt_server_is_running(void) {
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

void update_server(struct sftt_server *server) {
	int sock = 0;
	uint64_t current_ts = (uint64_t)time(NULL);
	int port = get_random_port();
	char buf[128];

	if (server->main_port != port) {
		sock = create_non_block_sock(&port);
		if (sock == -1) {
			return ;
		}

		sprintf(buf, "update main port and main sock. sock(%d -> %d), port(%d -> %d)",
			server->main_sock, sock, server->main_port, port);
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

int validate_user_info(struct client_session *session, struct sftt_packet *req, struct sftt_packet *resp)
{
	struct validate_req *v_req = (struct validate_req *)req->obj;
	struct client_info *client = NULL;

	add_log(LOG_INFO, "receive validate request|name: %s", v_req->name);
	char *md5_str = md5_printable_str(v_req->passwd_md5);
	if (md5_str) {
		add_log(LOG_INFO, "passwd md5: %s", md5_str);
		free(md5_str);
	}

	struct validate_resp v_resp;
	if (strcmp(v_req->name, "root") == 0) {
		v_resp.status = UVS_PASS;
		v_resp.uid = 19910930;
	} else {
		v_resp.status = UVS_BLOCK;
		v_resp.uid = 9527;
	}
	strncpy(v_resp.name, v_req->name, USER_NAME_MAX_LEN - 1);

	resp->type = PACKET_TYPE_VALIDATE_RSP;
	resp->obj = &v_resp;

	int ret = send_sftt_packet(session->connect_fd, resp);
	if (ret == -1) {
		printf("send validate response failed!\n");
		return -1;
	}

	session->cinfo.status = VALIDATED;
	generate_session_id(session->session_id, SESSION_ID_LEN);

	return 0;
}

void child_process_exception_handler(int sig) {
	printf("I'm child process and encountered segmental fault!\n");
	exit(-1);
}

void child_process_exit(int sig) {
	add_log(LOG_INFO, "I'm child process and exit for received signal.");
	exit(-1);
}

void *handle_client_session(void *args)
{
	struct client_session *session = (struct client_session *)args;
	int sock = session->connect_fd;

	struct sftt_packet *req = malloc_sftt_packet(VALIDATE_PACKET_MIN_LEN);
	struct sftt_packet *resp = malloc_sftt_packet(VALIDATE_PACKET_MIN_LEN);
	if (!req || !resp) {
		printf("cannot allocate resources from memory pool!\n");
		return NULL;
	}

	signal(SIGTERM, child_process_exit);
	signal(SIGSEGV, child_process_exception_handler);

	//if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
	//	printf("set sockfd to non-block failed!\n");
	//		return -1;
	//}

	int ret = 0;
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
			ret = validate_user_info(session, req, resp);
			if (ret == -1) {
				goto exit;
			}
			break;
		case PACKET_TYPE_FILE_NAME_REQ:
			
			break;
		case PACKET_TYPE_DATA_REQ:
			break;
		case PACKET_TYPE_FILE_END_REQ:
			break;
		case PACKET_TYPE_SEND_COMPLETE_REQ:
			break;
		default:
			break;
		}
	}

exit:
	session->status = EXITED;
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

	for (i = 0; i < MAX_CHILD_NUM; ++i) {
		if (server->sessions[i].status == EXITED) {
			return &server->sessions[i];
		}
	}

	return NULL;
}

#if 0
void register_child_process_info(int index, pid_t pid) {
	if (index >= MAX_CHILD_NUM) {
		return ;
	}

	struct sftt_server_info *ssi = get_sftt_server_info();
	if (ssi == NULL) {
		return ;
	}

	if (ssi->sessions[index].status == ACTIVE) {
		return ;
	}

	ssi->sessions[index].pid = pid;
	ssi->sessions[index].status = ACTIVE;
}

struct child_info *get_child_process_info(pid_t pid) {
	struct sftt_server_info *ssi = get_sftt_server_info();
	if (ssi == NULL) {
		return NULL;
	}

	int i = 0;
	for (i = 0; i < MAX_CHILD_NUM; ++i) {
		if (ssi->sessions[i].pid == pid) {
			return &ssi->sessions[i];
		}
	}

	return NULL;
}
#endif

void init_sessions(void)
{
	int i = 0;
	for (i = 0; i < MAX_CHILD_NUM; ++i) {
		server->sessions[i].status = EXITED;
	}
}

void main_loop(struct sftt_server *server) {
	int connect_fd = 0;
	pid_t pid = 0;
	int idx = 0;
	int ret;
	pthread_t child;
	struct client_session *session;

	server->status = RUNNING;
	init_sessions();

	while (1) {
		update_server(server);
		connect_fd = accept(server->main_sock, (struct sockaddr *)NULL, NULL);
		if (connect_fd == -1) {
			usleep(100 * 1000);
			continue;
		}
		if ((session = find_get_new_session()) == NULL) {
			close(connect_fd);
			continue;
		}
		add_log(LOG_INFO, "a client connected ...");

		ret = pthread_create(&child, NULL, handle_client_session, session);
		/*
		pid = fork();
		if (pid == 0){
			add_log(LOG_INFO, "I'm child");
			handle_client_session(connect_fd);
		} else if (pid < 0 ){
			printf("fork failed!\n");
		} else {
			register_child_process_info(idx, pid);
			wait(NULL);
		}
		*/
	}
}

int create_non_block_sock(int *pport) {
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

struct database *start_sftt_db_server(void) {
	return (void *)-1;
}

pid_t start_sftt_log_server(struct sftt_server *server) {
	pid_t pid = fork();
	if (pid == 0) {
		signal(SIGTERM, logger_exit);
		logger_daemon(server->conf.log_dir, PROC_NAME);
	} else {
		set_log_type(SERVER_LOG);
		sleep(1);
		return pid;
	}
}

bool create_sftt_server(struct sftt_server *server, char *store_path) {
	int port = 0;
	int sockfd = create_non_block_sock(&port);
	if (sockfd == -1) {
		return false;
	}

	server->status = READY;
	server->main_sock = sockfd;
	server->main_port = port;
	server->last_update_ts = (uint64_t)time(NULL);
	if (get_sftt_server_config(&(server->conf)) == -1) {
		return false;
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

	pid_t log_pid = start_sftt_log_server(server);
	if (log_pid < 0) {
		printf("cannot start log server!\n");
		return false;
	}

	bool ret = init_sftt_server_stat(log_pid);
	if (!ret) {
		printf(PROC_NAME " start failed! Because cannot init " PROC_NAME " server info.\n");
		return false;
	}

	return true;
}

int sftt_server_start(char *store_path) {
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

	if (daemon(1, 1) != 0) {
		printf("server cannot running in the background!\n");
		exit(-1);
	}

	struct sftt_server server;
	bool ret = create_sftt_server(&server, store_path);
	if (!ret) {
		printf(PROC_NAME " create server failed!\n");
		exit(-1);
	}

	add_log(LOG_INFO, PROC_NAME " is going to start in the background ...");

	signal(SIGTERM, sftt_server_exit);

	main_loop(&server);
}

struct sftt_server *sftt_server_construct(void)
{
	return (struct sftt_server *)mp_malloc(g_mp, sizeof(struct sftt_server));
}

void sftt_server_destruct(struct sftt_server *ptr)
{
	mp_free(g_mp, ptr);
}

int sftt_server_restart(char *store_path) {
	if (!sftt_server_is_running()) {
		printf("cannot restart " PROC_NAME ", because it is not running.\n");
		exit(-1);
	}

	if (strlen(store_path)) {
		if (access(store_path, W_OK)) {
			printf("write %s: Operation not permitted!\n", store_path);
			exit(-1);
		}
	}

	add_log(LOG_INFO, PROC_NAME " is going to restart ...");
	return 0;
}

/**
 * we should wait all children exit before sftt server exits.
 * todo: fix this bug.
 **/
int sftt_server_stop(void) {
	if (!sftt_server_is_running()) {
		printf("cannot stop " PROC_NAME ", because it is not running.\n");
		exit(-1);
	}

	struct sftt_server_stat *sss = get_sftt_server_stat();
	if (sss == NULL) {
		printf("cannot get " PROC_NAME " info!\n");
		return -1;
	}

	add_log(LOG_INFO, "log pid is: %d", sss->log_pid);
	add_log(LOG_INFO, "log is going to stop ...");
	kill(sss->log_pid, SIGTERM);

	/**
	*	All  of  these  system calls are used to wait for state changes
	*	in a child of the calling process, and obtain information about
	*	the child whose state has changed. A state change is considered
	*	to be: the child terminated; the child was stopped by a signal;
	*	or the child was resumed by a signal. In the case of a terminated
	*	child, performing a wait allows the system to release the resources
	*   associated with the child; if a wait is not performed, then
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
	//waitpid(ssi->log_pid, NULL, 0);
	printf(PROC_NAME " pid is: %d\n", sss->main_pid);
	printf(PROC_NAME " is going to stop ...\n");
	kill(sss->main_pid, SIGTERM);

	return 0;
}

void notify_all_child_to_exit(void) {
	int i = 0;
	for (i = 0; i < MAX_CHILD_NUM; ++i) {
		if (server->sessions[i].status != ACTIVE) {
			continue;
		}
		//kill(ssi->sessions[i].pid, SIGTERM);
	}
}

void sftt_server_exit(int sig) {
	printf(PROC_NAME " is exit ...!\n");
	notify_all_child_to_exit();
	free_sftt_server_stat();

	exit(-1);
}

static void version(void) {
    printf("version " VERSION ", Copyright (c) 2020-2020 zhou min, zhou qiang\n");
}

static void help(int exitcode) {
	version();
	printf("usage:\t" PROC_NAME " [option]\n"
		"\t" PROC_NAME " [start|restart dir]\n"
		"\t" PROC_NAME " [stop]\n"
		"\t" PROC_NAME " [status]\n"
		"\t" PROC_NAME " [db]\n");
	exit(exitcode);
}

bool parse_store_path(char *optarg, char *store_path, int max_len) {
	if (!optarg || !store_path) {
		return false;
	}

	strncpy(store_path, optarg, max_len);

	return true;
}

const char *status_desc(enum sftt_server_status status) {
	switch (status) {
	case RUNNING:
		return "running";
	case NOT_RUNNING:
		return "not running";
	default:
		return "unknown";
	}
}

void sftt_server_status(void) {
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
		"\tlog pid: %d\n"
		"\tlog path: %s\n"
		"\tlast update time: %s\n",
		status_desc(sss->status),
		sss->conf.store_path,
		sss->main_pid,
		sss->main_port,
		sss->main_sock,
		sss->log_pid,
		sss->conf.log_dir,
		ts_buf);
}

void execute_sql(char *sql, int flag) {

}

void sftt_server_db(void) {
	if (!sftt_server_is_running()) {
		printf("cannot connect " PROC_NAME " database, because " PROC_NAME " is not running!\n");
		printf("please start " PROC_NAME " first!\n");
		return ;
	}

	char sql[1024];

	while (1) {
		printf("sftt>>");
		fgets(sql, 1024, stdin);
		sql[strlen(sql) - 1] = 0;
		if (!strcmp(sql, "quit")) {
			exit(0);
		} else {
			execute_sql(sql, -1);
		}
	}
}

int main(int argc, char **argv) {
	int optind = 1;
	char *optarg = NULL;
	const struct sftt_option *opt = NULL;
	bool ret = false;
	char store_path[DIR_PATH_MAX_LEN];
	enum option_index opt_idx = UNKNOWN;

	if (argc < 2) {
		help(-1);
	}

	memset(store_path, 0, sizeof(store_path));
	for (;;) {
		if (optind >= argc) {
			break;
		}
		opt = lookup_opt(argc, argv, &optarg, &optind, sftt_server_opts);
		if (opt == NULL) {
			printf("invalid option: %s\n", argv[optind]);
			help(-1);
		}
		switch (opt->index) {
		case START:
			if (optarg) {
				ASSERT_STORE_PATH_LEN("start", optarg, DIR_PATH_MAX_LEN);
				ret = parse_store_path(optarg, store_path, DIR_PATH_MAX_LEN - 1);
				++optind;
			}
			opt_idx = START;
			break;
		case RESTART:
			if (optarg) {
				ASSERT_STORE_PATH_LEN("restart", optarg, DIR_PATH_MAX_LEN);
				ret = parse_store_path(optarg, store_path, DIR_PATH_MAX_LEN - 1);
				++optind;
			}
			opt_idx = RESTART;
			break;
		case STOP:
			opt_idx = STOP;
			break;
		case STATUS:
			opt_idx = STATUS;
			break;
		case DB:
			opt_idx = DB;
			break;
		default:
			printf("unknown parameter: %s\n", argv[optind]);
			help(-1);
			break;
		}
	}

	if (optind < argc) {
		printf("unknown parameters: %s\n", argv[optind]);
		help(-1);
	}

	switch (opt_idx) {
	case START:
		sftt_server_start(store_path);
		break;
	case RESTART:
		sftt_server_restart(store_path);
		break;
	case STOP:
		sftt_server_stop();
		break;
	case STATUS:
		sftt_server_status();
		break;
	case DB:
		sftt_server_db();
		break;
	}

	return 0;
}
