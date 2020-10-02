#ifndef __HEAD_SERVER__
#define __HEAD_SERVER__


void server_init_func();
int  sftt_server();
int  server_consult_block_size(int connect_fd,char *buff,int server_block_size);
void server_file_resv(int connect_fd , char * buff);









#endif
