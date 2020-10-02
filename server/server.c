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
#include "encrypt.h"

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
	//数据解密
        char *	mybuff = sftt_decrypt_func(buff,trans_len);
	int client_block_size = atoi(buff);
	int min_block_size = client_block_size < server_block_size ? client_block_size : server_block_size;

	sprintf(buff,"%d",min_block_size);
	int size = strlen(buff);
	//数据加密
	sftt_encrypt_func(buff,size);
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



void server_file_resv(int connect_fd , int consulted_block_size,sftt_server_config init_conf){
	int trans_len;
	char * block_buff = (char *) malloc(consulted_block_size * sizeof(char));
	if (block_buff == NULL ) {
		printf("创建缓冲区失败");
	}
	memset(block_buff,'\0',consulted_block_size);
	while (1){
		FILE * fd;
		char *data_buff = (char *) malloc(consulted_block_size * sizeof(char));
		memset(data_buff,'\0',consulted_block_size);
		while(1) {
			
			trans_len = recv(connect_fd, block_buff, consulted_block_size, 0);
			//char * tmp_buff = block_buff;
			//数据解密
			block_buff = sftt_decrypt_func(block_buff,trans_len);
			int i = 0;
			printf("第 %d 的内容为: %s\n",i,block_buff);
			printf("=======================================================================\n");
			if (trans_len <= 0) {
				printf("断开连接");
			}
			if (strncmp(block_buff,BLOCK_TYPE_FILE_NAME,strlen(BLOCK_TYPE_FILE_NAME)) == 0) {
				//创建传输文件

				fd = server_creat_file(block_buff,init_conf,data_buff);
				memset(block_buff,'\0',consulted_block_size);
			} else if (strncmp(block_buff,BLOCK_TYPE_DATA,strlen(BLOCK_TYPE_FILE_NAME)) == 0) {
				//接收数据 写入数据
				server_transport_data_to_file(fd,trans_len,block_buff);
				memset(block_buff,'\0',consulted_block_size);

			}else if (strncmp(block_buff,BLOCK_TYPE_FILE_END,strlen(BLOCK_TYPE_FILE_END)) == 0) {
				//一次文件传输完成
				printf("一次文件传输完成");
				fclose(fd);
				break;
			}else if (strncmp(block_buff,BLOCK_TYPE_SEND_COMPLETE,strlen(BLOCK_TYPE_SEND_COMPLETE)) == 0) {
				//整个传输过程结束
				printf("数据过程传输结束");
				exit(0);
			}else{
				//格式错误
				printf("传输格式有误");
				exit(0);
			}
		}

	}

}


void server_transport_data_to_file(FILE * fd,int size, char * block_buff ){
	char * tmp_buff = block_buff;
	int str_len = strlen(BLOCK_TYPE_DATA);
	fwrite(tmp_buff + str_len, 1, size - str_len, fd);

}


FILE * server_creat_file(char * block_buff, sftt_server_config  init_conf,char * data_buff){
	int str_len = strlen(block_buff);
	//const char * file_buff;
	char * tmp_file = block_buff;
	int i;
	FILE * fd;
	data_buff = strcat(data_buff,init_conf.store_path);
	for (i = 0; i < str_len; i++) {
		//if (block_buff[i] == '\0') {
	//		break;
	//	}
		if (i >= strlen(BLOCK_TYPE_FILE_NAME)) {
			strcat(data_buff,tmp_file);
			break;
		}else {
			tmp_file++;
			continue;
	  	}
	}
	//file_buff  = data_buff;
	//fd = fopen(data_buff,"r+");

	fd = fopen(data_buff,"w+");
	if (fd == NULL) {
		printf("文件创建失败");	
	}else{
		printf("%s 创建成功!",data_buff);
	
	}
	return fd;
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
			server_file_resv(connect_fd, consulted_block_size,init_conf );
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
