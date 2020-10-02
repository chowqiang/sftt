#ifndef __HEAD_SERVER__
#define __HEAD_SERVER__
#include "config.h"
#include <stdio.h>
void server_init_func();
int  sftt_server();
int  server_consult_block_size(int connect_fd,char *buff,int server_block_size);
void server_file_resv(int connect_fd , int consulted_block_size, sftt_server_config init_conf);
FILE *  server_creat_file(char * block_buff ,sftt_server_config  init_conf, char * data_buff);
void server_transport_data_to_file(FILE * fd,int size, char * data_buff );







#endif
