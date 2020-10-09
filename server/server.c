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
//#include "net_trans.h"

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
	//char * block_buff = (char *) malloc(consulted_block_size * sizeof(char));
	//if (block_buff == NULL ) {
	//	printf("创建缓冲区失败");
	//}
	//memset(block_buff,'\0',consulted_block_size);
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
				printf("server 等待 5次 结束进程！\n");
				exit(0);
			}
			//trans_len = recv(connect_fd, block_buff, consulted_block_size, 0);
			if (recv_sftt_packet(connect_fd, sp) != 0) {
				printf("数据传输失败 并等待");
				j ++ ;
				continue;
			}
			//char * tmp_buff = block_buff;
			//数据解密
			//block_buff = sftt_decrypt_func(block_buff,trans_len);
			//int i = 0;
			//printf("trans_len is %d\n",trans_len);
			printf("第%d次传输开始\n",i);
			printf("=======================================================================\n");
			if (trans_len <= 0) {
				fclose(fd);
				printf("断开连接\n");
				connected = 0;
				break;
			}
			if (strncmp(sp->type,BLOCK_TYPE_FILE_NAME,strlen(BLOCK_TYPE_FILE_NAME)) == 0) {
				//创建传输文件

				fd = server_creat_file(sp,init_conf,data_buff);
				//memset(block_buff,'\0',consulted_block_size);
				memset(data_buff,'\0',consulted_block_size);
				printf("到 1 区   \n");
				i++;
			} else if (strncmp(sp->type,BLOCK_TYPE_DATA,strlen(BLOCK_TYPE_FILE_NAME)) == 0) {
				//接收数据 写入数据
				server_transport_data_to_file(fd,sp);
				//memset(block_buff,'\0',consulted_block_size);
				printf("到 2 区   \n");
				i++;

			}else if (strncmp(sp->type,BLOCK_TYPE_FILE_END,strlen(BLOCK_TYPE_FILE_END)) == 0) {
				//一次文件传输完成
				printf("一次文件传输完成");
				server_transport_data_to_file(fd,sp);
				printf("到 3 区   \n");
				i++;
				fclose(fd);
				break;
			}else if (strncmp(sp->type,BLOCK_TYPE_SEND_COMPLETE,strlen(BLOCK_TYPE_SEND_COMPLETE)) == 0) {
				//整个传输过程结束
				printf("数据过程传输结束");
				printf("到 4 区   \n");
				i++;
				exit(0);
			}else{
				//格式错误
				printf("传输格式有误");
				printf("到 5 区   \n");
				i++;
				exit(0);
			}
		}

	}

}


void server_transport_data_to_file(FILE * fd,sftt_packet * sp ){
	//char * tmp_buff = block_buff;
	//int str_len = strlen(BLOCK_TYPE_DATA);
	int write_len=fwrite(sp->content, 1, sp->data_len, fd);
	printf("write len is %d", write_len);

}


FILE * server_creat_file(sftt_packet *sp, sftt_server_config  init_conf,char * data_buff){
	//int str_len = strlen(block_buff);
	//const char * file_buff;
	//char * tmp_file = sp->content;;
	int i;
	FILE * fd;
	data_buff = strcat(data_buff,init_conf.store_path);
	strcat(data_buff,sp->content);
			
	//file_buff  = data_buff;
	//fd = fopen(data_buff,"r+");
	is_exit(data_buff);

	fd = fopen(data_buff,"w+");
	if (fd == NULL) {
		printf("文件创建失败\n %s",data_buff);	
	}else{
		printf("%s 创建成功!\n",data_buff);
	
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
                        		printf("%s 目录创建失败！\n",tmp_path);
                		}
			}else {
				printf("%s 目录已经存在\n", tmp_path);
			}
			memset(tmp_path,'\0',str_len);
                        strcpy(tmp_path,filepath);
			printf("%s=====",tmp_path);
                        continue;
			
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
			server_file_resv(connect_fd, consulted_block_size,init_conf );
			
		
		} else if (pid < 0 ){
	
			printf("进程创建失败\n");

		} else {

			wait(NULL);
		}
		
	}
	close(socket_fd);

}
