#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h> 
#include <malloc.h>
#include <netinet/in.h>
#include "random_port.h"
#include "config.h"
#include "client.h"
#include "encrypt.h"
#include "net_trans.h"

typedef struct path_entry {
	char abs_path[FILE_NAME_MAX_LEN];
	char rel_path[FILE_NAME_MAX_LEN];
} path_entry;

typedef struct path_entry_list {
	path_entry entry;
	struct path_entry_list *next;
} path_entry_list;

path_entry *get_file_path_entry(char *file_name) {
	path_entry *pe = (path_entry *)malloc(sizeof(path_entry));
	if (pe == NULL) {
		return NULL;
	}	
	realpath(file_name, pe->abs_path);
	char *p = basename(pe->abs_path);
	strcpy(pe->rel_path, p);
	
	return pe;
}

void free_path_entry_list(path_entry_list *head) {
	path_entry_list *p = head, *q = head;
	while (p) {
		q = p->next;
		free(p);
		p = q;
	}
}
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

int consult_block_size_with_server(int sock, sftt_client_config *client_config) {
	char buffer[BUFFER_SIZE];
		
	memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
	sprintf(buffer, "%d", client_config->block_size);
	//printf("client block size is : %d\n", client_config.block_size);
	sftt_encrypt_func((char *)buffer, BUFFER_SIZE); 
	
	int ret = send(sock, buffer, BUFFER_SIZE, 0);
	if (ret <= 0) {
		return -1;
	}
	
	memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
	ret = recv(sock, buffer, BUFFER_SIZE, 0); 
	if (ret <= 0) {
		return -1;
	}
	sftt_decrypt_func(buffer, ret);
	int consulted_block_size = atoi(buffer);
	//printf("consulted block size is: %d\n", consulted_block_size);
	
	return consulted_block_size; 
}

int send_single_file(int sock, sftt_packet *sp, path_entry *pe) {
	FILE *fp = fopen(pe->abs_path, "rb");
	if (fp == NULL) {
		printf("Error. cannot open file: %s\n", pe->abs_path);
		return -1;
	}
	
	printf("sending file %s\n", pe->abs_path);

	strcpy(sp->type, BLOCK_TYPE_FILE_NAME);
	sp->data_len = strlen(pe->rel_path);
	strcpy(sp->content, pe->rel_path);
	int ret = send_sftt_packet(sock, sp);
	if (ret == -1) {
		printf("send file name block failed!\n");
		return -1;
	}
	
	int read_count = 0, i = 0, j = 0;
	int prefix_len = strlen(BLOCK_TYPE_DATA);
	do {
		printf("%d-th transport ...\n", ++j);
		strcpy(sp->type, BLOCK_TYPE_DATA);
		read_count = fread(sp->content, 1, sp->block_size, fp);		
		printf("read block size: %d\n", read_count);
		if (read_count < 1) {
			break;
		}

		/*
		for (i = 0; i < read_count; ++i) {
			printf("%c", buffer[i]);
		}
		*/
		sp->data_len = read_count;
		ret = send_sftt_packet(sock, sp);
		if (ret == -1) {
			printf("send data block failed!\n");
			return -1;
		}
	
	} while (read_count == sp->block_size);

	strcpy(sp->type, BLOCK_TYPE_FILE_END);
	sp->data_len = 0;
	ret = send_sftt_packet(sock, sp);
	if (ret == -1) {
		printf("send file end block failed!\n");
		return -1;
	}

	printf("sending %s done!\n", pe->abs_path);

	return 0;
}

path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix) {
	/*
	path_entry_list *head = (path_entry_list *)malloc(sizeof(path_entry_list));
	if (head == NULL) {
		return NULL;
	}
	*/
	path_entry_list *head = NULL;
	path_entry_list *current_entry = NULL;
	path_entry_list *sub_list = NULL;

	char dir_abs_path[FILE_NAME_MAX_LEN];
	char dir_rel_path[FILE_NAME_MAX_LEN];
	char tmp_path[FILE_NAME_MAX_LEN];

	realpath(file_name, dir_abs_path);
	char *p = basename(dir_abs_path);
	sprintf(dir_rel_path, "%s/%s", prefix, p);
		
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
 
	if ((dp = opendir(file_name)) == NULL) {
		printf("cannot open dir: %s\n", file_name);
		return NULL;
	}
	
	chdir(file_name);
	while ((entry = readdir(dp)) != NULL) {
		sprintf(tmp_path, "%s/%s", dir_abs_path, entry->d_name);
		lstat(tmp_path, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}
			//printf("dir tmp path: %s\n", tmp_path);
			sub_list = get_dir_path_entry_list(tmp_path, dir_rel_path);	
			if (sub_list == NULL) {
				continue;	
			}

			if (current_entry == NULL) {
				current_entry = head = sub_list;
			} else {
				current_entry->next = sub_list;
			}

			while (current_entry->next) {
				current_entry = current_entry->next;
			}				
				
		} else {
			path_entry_list *node = (path_entry_list *)malloc(sizeof(path_entry_list));
			if (node == NULL) {
				continue;
			}

			sprintf(node->entry.abs_path, "%s/%s", dir_abs_path, entry->d_name);
			sprintf(node->entry.rel_path, "%s/%s", dir_rel_path, entry->d_name);
			//printf("file: %s\n", node->entry.abs_path);
			node->next = NULL;

			if (current_entry == NULL) {
				current_entry = head = node;
			} else {
				current_entry->next = node;
				current_entry = node;
			}
		}
	}
	closedir(dp);	

	return head;
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

	printf("reading config ...\n");
	sftt_client_config client_config;
	int ret = get_sftt_client_config(&client_config);
	if (ret == -1) {
		printf("get client config failed!\n");
		return -1;
	}
	printf("reading config done!\nconfigured block size is: %d\n", client_config.block_size);

	int sock = create_client();
	if (sock == -1) {
		printf("Error. create client failed!\n");
		return -1;
	} else {
		printf("create client successfully!\n");
	}

	//sftt_packet *sp = malloc_sftt_packet(BUFFER_SIZE);
	
	printf("consulting block size with server ...\n");
	int consulted_block_size = consult_block_size_with_server(sock, &client_config);
	if (consulted_block_size < 1) {
		printf("consult block size with server failed!\n");
		return -1;
	}
	printf("consulting block size done!\nconsulted block size is: %d\n", consulted_block_size);	

	/*
	char *buffer = (char *)malloc(sizeof(char) * consulted_block_size);
	if (buffer == NULL) {
		printf("create buffer for transporting failed!\n");
		return -1;
	}
	*/
	sftt_packet *sp = malloc_sftt_packet(consulted_block_size);
	if (sp == NULL) {
		printf("malloc sftt packet failed!\n");
		return -1;
	}

	if (is_file(target)) {
		path_entry *pe = get_file_path_entry(argv[1]);	
		if (pe == NULL) {
			printf("get file path entry failed!\n");
			return -1;
		}

		send_single_file(sock, sp, pe);

		free(pe);

	} else if (is_dir(target)) {
		char prefix[1] = {0};
		path_entry_list *pes = get_dir_path_entry_list(argv[1], prefix);
		if (pes == NULL) {
			printf("get dir path entry list failed!\n");
			return -1;
		}
		
		path_entry_list *p = pes;
		while (p) {
			send_single_file(sock, sp, &(p->entry));
			p = p->next;
		}
		
		
		strcpy(sp->type, BLOCK_TYPE_SEND_COMPLETE);
		sp->data_len = 0;
		int ret = send_sftt_packet(sock, sp);
		if (ret == -1) {
			printf("send complete end block failed!\n");
		}	
			
		free_path_entry_list(pes);
	}

	free_sftt_packet(&sp);
        close(sock);

	return 0;
}
