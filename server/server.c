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
#include "server.h"
#include "encrypt.h"
#include "config.h"
#include "random_port.h"
#include "version.h"
//#include "net_trans.h"

#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)  

//#define BUFFER_SIZE  10240

static bool update_sftt_server_info(sftt_server_info *info, int create_flag);

static void sftt_server_exit(int sig);

void server_init_func(sftt_server_config *server_config){
	DIR  *mydir = NULL;
	if (get_sftt_server_config(server_config) != 0) {
		printf(PROC_NAME ": get server config failed!\n");
		exit(0);
	}
	char *filepath = server_config->store_path;
	printf("conf  block_size is %d\n", server_config->block_size);
	printf("store path: %s\n",filepath);
	if((mydir= opendir(filepath))==NULL) {
		int ret = mkdir(filepath, MODE);
		if (ret != 0) {
			printf(PROC_NAME ": create work dir failed!\n");
			exit(0);	
		}
	 }

}

int  server_consult_block_size(int connect_fd,char *buff,int server_block_size){
	int trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
	if (trans_len <= 0 ) {
		printf("consult block size recv failed!\n");
		exit(0);
	}

	char *	mybuff = sftt_decrypt_func(buff,trans_len);
	int client_block_size = atoi(buff);
	int min_block_size = client_block_size < server_block_size ? client_block_size : server_block_size;

	sprintf(buff,"%d",min_block_size);
	int size = strlen(buff);
	sftt_encrypt_func(buff,size);
	send(connect_fd,buff,BUFFER_SIZE,0);

	return min_block_size;

}

void server_file_resv(int connect_fd , int consulted_block_size,sftt_server_config init_conf){
	int trans_len;
	sftt_packet *sp = malloc_sftt_packet(consulted_block_size);
	int connected = 1;
	while (connected){
		FILE * fd;
		int i = 0 ;
		int j = 0 ; 
		char *data_buff = (char *) malloc(consulted_block_size * sizeof(char));
		memset(data_buff,'\0',consulted_block_size);
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
			printf("recv %d-th\n",i);
			printf("=======================================================================\n");
			if (trans_len <= 0) {
				fclose(fd);
				printf("recev failed!\n");
				connected = 0;
				break;
			}
			switch (sp->type) {
			case BLOCK_TYPE_FILE_NAME:
				fd = server_creat_file(sp,init_conf,data_buff);
				memset(data_buff,'\0',consulted_block_size);
				printf("get file name packet\n");
				i++;
				break;
			case BLOCK_TYPE_DATA:
				server_transport_data_to_file(fd,sp);
				printf("get file type packet\n");
				i++;
				break;
			case BLOCK_TYPE_FILE_END:
				printf("get file end packet\n");
				server_transport_data_to_file(fd,sp);
				i++;
				fclose(fd);
				break;
			case BLOCK_TYPE_SEND_COMPLETE:
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

void server_transport_data_to_file(FILE * fd,sftt_packet * sp ){
	int write_len=fwrite(sp->content, 1, sp->data_len, fd);
	printf("write len is %d", write_len);
}


FILE * server_creat_file(sftt_packet *sp, sftt_server_config  init_conf,char * data_buff){
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
	sftt_server_config  init_conf;
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
   printf("Caught signal %d, coming out...\n", signum);
}

bool init_sftt_server_info(sftt_server *server) {
	sftt_server_info ssi = {
		.pid = getpid(),
		.status = RUNNING,
	};
	strcpy(ssi.store_path, server->conf.store_path);

	return update_sftt_server_info(&ssi, 1);
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

sftt_server_info *get_sftt_server_info(void) {
	return (sftt_server_info *)get_sftt_server_shmaddr(0);
}

static bool update_sftt_server_info(sftt_server_info *info, int create_flag) {
	void *shmaddr = get_sftt_server_shmaddr(create_flag);
	if (shmaddr == NULL) {
		printf("get shared memory for " PROC_NAME " failed!\n");
		return false;
	}

	memcpy(shmaddr, info, sizeof(sftt_server_info));
	shmdt(shmaddr);

	return true;
}

bool remove_sftt_server_info(void) {
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

	sftt_server_info *ssi = shmat(shmid, NULL, 0);
    if (ssi == (void *)(-1)) {
		return false;
    }

	return ssi->status == RUNNING;
}

void update_server(sftt_server *server) {
	int sock = 0;
	uint64_t current_ts = (uint64_t)time(NULL);
	if ((current_ts - server->last_update_ts) >= server->conf.update_th) {
		sock = create_non_block_sock();
		if (sock != -1) {
			close(server->main_sock);
			server->main_sock = sock;
		}
	}
	server->last_update_ts = current_ts;
}

void deal_client_session(int sock) {
	while (1) {
		;
	}
}

void main_loop(sftt_server *server) {
	int connect_fd = 0;
	pid_t pid = 0;

	while (1) {
		update_server(server);
		connect_fd = accept(server->main_sock, (struct sockaddr *)NULL, NULL);
		if (connect_fd == -1) {
			usleep(100 * 1000);
			continue;
		}
		pid = fork();
		if (pid == 0){
			printf("I'm child\n");
			deal_client_session(connect_fd);
		} else if (pid < 0 ){
			printf("fork failed!\n");
		} else {
			wait(NULL);
		}
	}
}

int create_non_block_sock(void) {
	int	sockfd;
	struct sockaddr_in serveraddr;
	int	port = get_random_port();
	printf("random port is %d\n", port);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("create socket filed!\n");
		return -1;
	}

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
		printf("set sockfd to non-block failed!\n");
		return -1;
	}

	memset(&serveraddr, 0 ,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("bind socket error!\n");
		return -1;
	}

	if (listen(sockfd, 10) == -1){
		printf("listen socket error\n");
		return -1;
	}

	return sockfd;
}

bool create_sftt_server(sftt_server *server, char *store_path) {
	int sockfd = create_non_block_sock();
	if (sockfd == -1) {
		return false;
	}

	server->main_sock = sockfd;
	server->last_update_ts = (uint64_t)time(NULL);
	if (get_sftt_server_config(&(server->conf)) == -1) {
		return false;
	}

	if (strlen(store_path)) {
		strcpy(server->conf.store_path, store_path);
	}

	bool ret = init_sftt_server_info(server);
	if (!ret) {
		printf(PROC_NAME " start failed! Because cannot init " PROC_NAME " server info.\n");
		exit(-1);
	}

	return true;
}

int sftt_server_start(char *store_path) {
	if (sftt_server_is_running()) {
		printf("cannot start " PROC_NAME ", because it has been running.\n");
		exit(-1);
	}

	if (access(store_path, W_OK)) {
		printf("write %s: Operation not permitted!\n", store_path);
		exit(-1);
	}

	if (daemon(1, 1) != 0) {
		printf("server cannot running in the background!\n");
		exit(-1);
	}

	sftt_server server;
	bool ret = create_sftt_server(&server, store_path);
	if (!ret) {
		printf(PROC_NAME " create server failed!\n");
		exit(-1);
	}

	printf(PROC_NAME " is going to start in the background ...\n");

	signal(SIGTERM, sftt_server_exit);

	main_loop(&server);
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

	printf(PROC_NAME " is going to restart ...\n");
	//return update_sftt_server_info(NULL);
	return 0;
}

int sftt_server_stop(void) {
	if (!sftt_server_is_running()) {
		printf("cannot stop " PROC_NAME ", because it is not running.\n");
		exit(-1);
	}

	sftt_server_info *ssi = get_sftt_server_info();
	if (ssi == NULL) {
		printf("cannot get " PROC_NAME " info!\n");
		return -1;
	}

	printf(PROC_NAME " pid is: %d\n", ssi->pid);
	printf(PROC_NAME " is going to stop ...\n");
	kill(ssi->pid, SIGTERM);

	return 0;
}

void sftt_server_exit(int sig) {
	printf(PROC_NAME " is exit ...!\n");
	remove_sftt_server_info();
	exit(-1);
}

static void version(void) {
    printf("version " VERSION ", Copyright (c) 2020-2020 zhou min, zhou qiang\n");
}

static void help(int exitcode) {
	version();
	printf("usage:\t" PROC_NAME " [option]\n"
		"\t" PROC_NAME " [start|restart dir]\n"
		"\t" PROC_NAME " [stop]\n");
	exit(exitcode);
}

bool parse_store_path(char *optarg, char *store_path, int max_len) {
	if (!optarg || !store_path) {
		return false;
	}

	strncpy(store_path, optarg, max_len);

	return true;
}

int main(int argc, char **argv) {
	int optind = 1;
	char *optarg = NULL;
	const sftt_option *opt = NULL;
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
				opt_idx = START;
			}
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
	}

	return 0;
}
