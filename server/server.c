#include "server.h"
#include <stdio.h>
#include <sys/socket.h>  
#include <netinet/in.h>  
#include "random_port.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include<sys/types.h> 
#include<sys/stat.h>  

#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)  

//#define BUFFER_SIZE  10240


void server_init_func(sftt_server_config *server_config){
	DIR  *mydir = NULL;
	//sftt_server_config server_config;
	if (get_sftt_server_config(server_config) != 0) {
		printf("初始化失败");
		exit(0);
	}
	char *filepath = server_config->store_path;
	printf("conf  block_size is %d\n", server_config->block_size);
	printf("store path: %s\n",filepath);
	if((mydir= opendir(filepath))==NULL) {
		int ret = mkdir(filepath, MODE);
		if (ret != 0) {
			printf("目录创建失败!");
			exit(0);	
		}
	 } 	

}

int  server_consult_block_size(int connect_fd,char *buff,int server_block_size){
	int trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
	if (trans_len <= 0 ) {
		printf("首次断开连接");
		exit(0);
	}
	int client_block_size = atoi(buff);
	int min_block_size = client_block_size < server_block_size ? client_block_size : server_block_size;

	sprintf(buff,"%d",min_block_size);
	// send
	send(connect_fd,buff,BUFFER_SIZE,0);
	return min_block_size;

}
int  sftt_server(){
	int		socket_fd;  
	struct 		sockaddr_in     serveraddr;
	int		port = get_random_port();
	printf("port is %d\n", port);

	if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		printf("socket connetion filed ");
		exit(0);
	}
	
	memset(&serveraddr, 0 ,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;  
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	if( bind(socket_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("bind error");  
		exit(0);  
	}
	if( listen(socket_fd, 10) == -1){
		printf("listen socket error\n");
		exit(0);  
	}  
	

	printf("开始等待数据传输。。。。。\n");
	return socket_fd;

	
	
}



void server_file_resv(int connect_fd , char * buff){
	int trans_len;
	while (1){
		while(1) {
			
			trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
		}

	}

}



int main(){
	int	socket_fd = sftt_server();
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
			//server_file_resv(int connect_fd , char * buff);
			//trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
			//if (trans_len <= 0) {
			//	printf("客户端断开连接\n")	
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//}
			
			//while(1){
                	//	printf("正在传输数据：\n");
			//	trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
			//	printf("translen is  %d\n", trans_len);
			//	if(trans_len <= 0){
			//		printf("客户端断开连接\n");
			//		break;
			//	}
			//	buff[trans_len] = '\0';  
			//	printf("recv msg from client: %s\n", buff); 
			//	if(strcmp(buff, quit) == 0){
			//		close(connect_fd);
			//		break;
			//	}
			//:}
			
		
		} else if (pid < 0 ){
	
			printf("进程创建失败\n");

		} else {

			//waitpid(pid);
			;
		}
		
	}
	close(socket_fd);

}
