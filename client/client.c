#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <malloc.h>
#include <netinet/in.h>
#include "random_port.h"
#include "config.h"
#include "client.h"
#include "encrypt.h"

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

int create_client() {
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;  //使用IPv4地址
    	serv_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);  //具体的IP地址
	int ret = -1;
	int port = get_cache_port();
	printf("cache port is %d\n", port);

	if (port != -1) {
		serv_addr.sin_port = htons(port);  //端口

		ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	}
	
	if (ret == -1) {
		port = get_random_port();
		printf("random port is %d\n", port);
		serv_addr.sin_port = htons(port);	
		 	
	 	ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	} 
	
	if (ret == -1) {
		return -1;
	}
	
	set_cache_port(port);

	return sock;
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
	if (strlen(file_name) > FILE_NAME_MAX_LEN) {
		return NULL;
	}
	file_input_stream *fis = (file_input_stream *)malloc(sizeof(file_input_stream));
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

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Error. Usage: %s file|dir\n", argv[0]);
		return -1;
	}
	char *target = argv[1];
	if (strlen(target) > FILE_NAME_MAX_LEN) {
		printf("Error. File name too long: %s\n", argv[1]);
		return -1;
	} 

	file_input_stream *fis = create_file_input_stream(target);
	if (fis == NULL) {
		printf("create file input stream object failed!\n");
		return -1;
	}

	int sock = create_client();
	printf("begin to send file content ...\n");

	int read_count = 0;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
		
	sftt_client_config client_config;
	int ret = get_sftt_client_config(&client_config);
	if (ret == -1) {
		printf("get client config failed!\n");
		return -1;
	}

	sprintf(buffer, "%d", client_config.block_size);
	printf("client block size is : %d\n", client_config.block_size);
	send(sock, buffer, BUFFER_SIZE, 0);
	recv(sock, buffer, BUFFER_SIZE, 0); 
	printf("consulted block size is: %d\n", atoi(buffer));
	return 0;
		
	// send file name first.
	strcpy(buffer, BLOCK_TYPE_FILE_NAME); 	
	strcpy(buffer + strlen(BLOCK_TYPE_FILE_NAME), target);	
	sftt_encrypt_func((char *)buffer); 
	send(sock, buffer, strlen(buffer), 0);
	usleep(1000);

	// send file content later.
	do {
		strcpy(buffer, BLOCK_TYPE_DATA);
		read_count = fis->get_next_buffer(fis, buffer + strlen(BLOCK_TYPE_DATA), BUFFER_SIZE - strlen(BLOCK_TYPE_DATA));
		if (read_count < 1) {
			break;
		}  
		sftt_encrypt_func((char *)buffer); 
		send(sock, buffer, read_count, 0);
		sleep(60);
	} while (read_count == BUFFER_SIZE);

	printf("done!\n", buffer);
   
    	//关闭套接字
        close(sock);

	destory_file_input_stream(fis);

	return 0;	
}
