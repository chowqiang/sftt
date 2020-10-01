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

//#define BUFFER_SIZE  10240

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
		printf("listen socket error");
		exit(0);  
	}  
	

	printf("开始等待数据传输。。。。。");
	return socket_fd;

	
	
}







int main(){
	int	socket_fd = sftt_server();
	int	connect_fd;
	int	trans_len;
	pid_t   pid;
        char	buff[BUFFER_SIZE];
	char    quit[BUFFER_SIZE] = {'q','u','i','t'};
	while(1){
		if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){  
		printf("connect filed");	
		continue;
		}
		pid = fork();
		if ( pid == 0 ){
			;
			while(1){
                		printf("正在传输数据：\n");
				trans_len = recv(connect_fd, buff, BUFFER_SIZE, 0);
				printf("translen is  %d\n", trans_len);
				if(trans_len <= 0){
					printf("客户端断开连接\n");
					break;
				}
				buff[trans_len] = '\0';  
				printf("recv msg from client: %s\n", buff); 
				if(strcmp(buff, quit) == 0){
					close(connect_fd);
					break;
				}
			}
			
		
		} else if (pid < 0 ){
	
			printf("进程创建失败");

		} else {

			//waitpid(pid);
			;
		}
		
	}
	close(socket_fd);

}
