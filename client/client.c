#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h> 
#if __linux__
#include <malloc.h>
#endif
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include "random_port.h"
#include "config.h"
#include "client.h"
#include "encrypt.h"
#include "net_trans.h"
#include "validate.h"
#include "debug.h"

extern int errno;

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

int new_connect(char *ip, int port, sftt_client_config *config, sock_connect *psc) {
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;  //使用IPv4地址
    	serv_addr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
	serv_addr.sin_port = htons(port);  //端口
	int ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		return -1;
	}

	printf("consulting block size with server ...\n");
	int consulted_block_size = consult_block_size_with_server(sock, config);
	if (consulted_block_size < 1) {
		printf("Error. consult block size with server failed!\n");
		return -1;
	}
	printf("consulting block size done!\nconsulted block size is: %d\n", consulted_block_size);	

	psc->sock = sock;
	psc->block_size = consulted_block_size;

	return 0;
}

sftt_client *create_client(char *ip, sftt_client_config *config, int connects_num) {
	sftt_client *client = (sftt_client *)malloc(sizeof(sftt_client));
	if (client == NULL) {
		return NULL;
	}
	memset(client, 0, sizeof(sftt_client));

	strcpy(client->ip, ip);
	printf("server ip is: %s\n", ip);

	int port = get_cache_port();
	printf("cache port is %d\n", port);
	
	int i = 0, ret = 0;
	sock_connect sc;
	for (i = 0; i < connects_num; ++i) {
		ret = new_connect(ip, port, config, &sc);
		if (ret == -1) {
			break;
		}	
		client->connects[client->connects_num++] = sc;
	}
	
	if (i > 0) {
		printf("sock connects num is: %d\n", i);
		client->port = port;
		return client;
	}
	
	port = get_random_port();
	printf("random port is %d\n", port);
	for (; i < connects_num; ++i) {
		ret = new_connect(ip, port, config, &sc);
		if (ret == -1) {
			break;
		}	
		client->connects[client->connects_num++] = sc;
	}

	if (i > 0) {
		printf("sock connects num is: %d\n", i);
		client->port = port;
		set_cache_port(port);
		return client;
	}

	fprintf(stderr, "Value of errno: %d\n", errno);
	fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	free(client);
	
	return NULL;
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
		printf("Error. send file name block failed!\n");
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
			printf("Error. send data block failed!\n");
			return -1;
		}
	
	} while (read_count == sp->block_size);

	strcpy(sp->type, BLOCK_TYPE_FILE_END);
	sp->data_len = 0;
	ret = send_sftt_packet(sock, sp);
	if (ret == -1) {
		printf("Error. send file end block failed!\n");
		return -1;
	}

	printf("sending %s done!\n", pe->abs_path);


	return 0;
}

path_entry *get_dir_path_entry_array(char *file_name, char *prefix, int *pcnt) {
	*pcnt = 0;
	path_entry_list *head = get_dir_path_entry_list(file_name, prefix);			
	if (head == NULL) {
		return NULL;
	}

	int count = 0;
	path_entry_list *p = head;
	while (p) {
		++count;
		p = p->next;
	}

	path_entry *array = (path_entry *)malloc(sizeof(path_entry) * count);
	if (array == NULL) {
		free_path_entry_list(head);
		return NULL;
	}
	
	int i = 0;
	p = head;
	for (i = 0; i < count; ++i) {
		if (p == NULL) {
			printf("Error. Unexpected fatal when copy path entry!\n");
		}
		strcpy(array[i].abs_path, (p->entry).abs_path);
		strcpy(array[i].rel_path, (p->entry).rel_path);
		p = p->next;
	}
	
	*pcnt = count;

	free_path_entry_list(head);
	
	return array;
}

path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix) {
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
		printf("Error. cannot open dir: %s\n", file_name);
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

void destory_sftt_client(sftt_client *client) {
	if (client == NULL) {
		return ;
	}
	int i = 0;
	for (i = 0; i < client->connects_num; ++i) {
		close(client->connects[i].sock);
	}
}

void usage(char *exec_file) {
	fprintf (stdout, "\nUsage: %s -h ip <input_file>\n\n", exec_file);
}

void *send_files_by_thread(void *args) {
	thread_input_params *tip = (thread_input_params *)args;
	sftt_packet *sp = malloc_sftt_packet((tip->connect).block_size);
	if (sp == NULL) {
		printf("Error. malloc sftt packet failed!\n");
		return (void *)-1;
	}
	int i = 0, ret = 0;
	for (i = tip->index; i < tip->pe_count; i += tip->step) {
		ret = send_single_file((tip->connect).sock, sp, tip->pes + i);
		if (ret == -1) {
			printf("Error. send single file in thread %d failed! abs path: %s, rel path: %s\n", tip->index, tip->pes[i].abs_path, tip->pes[i].rel_path);
			break;
		}
	}

	send_complete_end_packet((tip->connect).sock, sp);

	return NULL;
}

int send_complete_end_packet(int sock, sftt_packet *sp) {
	strcpy(sp->type, BLOCK_TYPE_SEND_COMPLETE);
	sp->data_len = 0;
	int ret = send_sftt_packet(sock, sp);
	if (ret == -1) {
		printf("Error. send complete end block failed!\n");
		return -1;
	}	

	return 0;
}

int send_multiple_file(sftt_client *client, path_entry *pes, int count) {
	if (count == 0) {
		return -1;
	}
	
	int i = 0, ret = 0;
	pthread_t thread_ids[CLIENT_MAX_CONNECT_NUM];
	thread_input_params tips[CLIENT_MAX_CONNECT_NUM];
	for (i = 0; i < client->connects_num; ++i) {
		tips[i].connect = client->connects[i];
		tips[i].index = i;
		tips[i].step = client->connects_num;
		tips[i].pes = pes;
		tips[i].pe_count = count;
		ret = pthread_create(thread_ids + i, NULL, send_files_by_thread, tips + i);
		if (ret != 0) {
			printf("Error. thread %d create failed!\n", i);
			return -1;
		}
	} 

	void *pret = NULL;
	for (i = 0; i < client->connects_num; ++i) {
		pthread_join(thread_ids[i], &pret);
		if(pret == (void*)-1)
		{
		    printf("Error. thread %d exit failed!\n", i);
		    ret = -1;
		}
	}	
	

	return 0;
}

int find_unfinished_session(path_entry *pe, char *ip) {
	return 0;		
}

int file_trans_session_diff(file_trans_session *old_session, file_trans_session *new_seesion) {
	return 0;
}

int dir_trans_session_diff(dir_trans_session *old_session, dir_trans_session *new_session) {
	return 0;
}

int save_trans_session(sftt_client *client) {
	return 0;
}


int main2(int argc, char **argv) {
	if (argc < 4) {
		usage(argv[0]);
		return -1;
	}

	char *ip = NULL;
	char *target = NULL; 
	int i = 1;
	for (; i < argc; ++i) {
		if (strcmp(argv[i], "-h") == 0) {
			++i;
			if (i >= argc) {
				goto PARAMS_ERROR;
			}	
			ip = argv[i]; 
			continue;
		} 
		if (i == argc - 1) {
			target = argv[i];	
		}
	}

	if (ip == NULL || !is_valid_ipv4(ip)) {
		printf("Error. ip is invalid: %s\n", (ip == NULL) ? "NULL" : ip);
		return -1;
	}

	if (target == NULL) {
		goto PARAMS_ERROR;
	}
	if (strlen(target) > FILE_NAME_MAX_LEN) {
		printf("Error. File name too long: %s\n", target);
		return -1;
	} 

	printf("reading config ...\n");
	sftt_client_config client_config;
	int ret = get_sftt_client_config(&client_config);
	if (ret == -1) {
		printf("Error. get client config failed!\n");
		return -1;
	}
	printf("reading config done!\nconfigured block size is: %d\n", client_config.block_size);

	int connects_num = 0; 
	sftt_client *client = NULL;
	if (is_file(target)) {
		connects_num = 1;
		client = create_client(ip, &client_config, connects_num);
		if (client == NULL) {
			printf("Error. create client failed!\n");
			return -1;
		} else {
			printf("create client successfully!\n");
		}

		path_entry *pe = get_file_path_entry(target);	
		if (pe == NULL) {
			printf("Error. get file path entry failed!\n");
			return -1;
		}

		sftt_packet *sp = malloc_sftt_packet(client->connects[0].block_size);
		if (sp == NULL) {
			printf("Error. malloc sftt packet failed!\n");
			return -1;
		}

		send_single_file(client->connects[0].sock, sp, pe);
		send_complete_end_packet(client->connects[0].sock, sp);

		free_sftt_packet(&sp);
		free(pe);

	} else if (is_dir(target)) {
		connects_num = CLIENT_MAX_CONNECT_NUM;
		client = create_client(ip, &client_config, connects_num);
		if (client == NULL) {
			printf("Error. create client failed!\n");
			return -1;
		} else {
			printf("create client successfully!\n");
		}

		char prefix[1] = {0};
		int count = 0;
		path_entry *pes = get_dir_path_entry_array(target, prefix, &count);
		if (pes == NULL) {
			printf("Error. get dir path entry list failed!\n");
			return -1;
		}

		send_multiple_file(client, pes, count);

		free(pes);
	}

	destory_sftt_client(client);

	return 0;

PARAMS_ERROR:
	usage(argv[0]);

	return -1;
}

void execute_cmd(char *cmd, int flag) {
	printf("%s\n", cmd);

	DEBUG(("debug: %s\n", cmd));
}

const sftt_option *lookup_opt(int argc, char **argv, char **optarg, int *optind) {
	if (optind >= argc) {
		return NULL;
	}
	sftt_option *opt = sftt_opts;
	for (;;) {
		if (!opt->name) {
			return NULL;
		}
		if (!strcmp(opt->name, argv[optind])) {
			break;
		}
	}
	(*optind)++;
	if (opt->flags & HAS_ARG) {
		if (optind >= argc) {
			return NULL;
		}
		*optarg = argv[*optind];
		(*optind)++;
	}

	return opt;
}

void get_passwd_input(char *passwd) {
	int max_passwd_len = 32;
	int i = 0;
	for(i = 0; i < max_passwd_len; i++) {
		passwd[i] = getch();
        if (passwd[i] == '\x0d') {
            passwd[i]='\0';
            break;
        }
        printf("*");
    }
	printf("\n");
}

int main(int argc, char **argv) {
	char cmd[1024];
	int optind = 1;
	char *optarg = NULL;
	bool has_passwd_opt = false;
	char user_name[1024];
	char password[1024];
	char host[1024];

	memset(user_name, 0, sizeof(user_name));
	memset(password, 0, sizeof(password));
	memset(host, 0, sizeof(host));
	for (;;) {
		if (optind >= argc) {
			break;
		}
		opt = lookup_opt(argc, argv, &optarg, &optind);
		if (opt == NULL) {
			printf("invalid option");
			return -1;
		}
		switch (opt->index) {
		case USER:
			user_name_parse(optarg, user_name);
			break;
		case HOST:
			host_parse(optarg, host);
			break;
		case PASSWORD:
			has_passwd_opt = true;
			break;
		}
	}
	if (has_passwd_opt) {
		get_passwd_input(password);
	}

	printf("host: %s\n", host);
	printf("your name: %s\n", user_name);
	printf("your password: %s\n", password);

	while (1) {
		printf("sftt>>");
		fgets(cmd, 1024, stdin);
		cmd[strlen(cmd) - 1] = 0;
		if (!strcmp(cmd, "quit")) {
			exit(0);
		} else {
			execute_cmd(cmd, -1);
		}
	}
	return 0;

}
