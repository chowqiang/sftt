#include <stdlib.h>
#if __linux__ 
#include <malloc.h>
#endif
#include <sys/socket.h>
#include <stdio.h>
#include "net_trans.h"
#include "config.h"
#include "encrypt.h"

sftt_packet *malloc_sftt_packet(int block_size) {
	sftt_packet *sp = (sftt_packet *)malloc(sizeof(sftt_packet));
	if (sp == NULL) {
		return NULL;
	}	
	memset(sp, 0, sizeof(*sp));

	sp->content = (char *)malloc(sizeof(char) * block_size);
	if (sp->content == NULL) {
		free(sp);
		return NULL;
	}
	sp->block_size = block_size;	
	
	return sp;
}

int send_sftt_packet(int sock, sftt_packet *sp) {
	char header[BLOCK_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 
	//int packet_size = header_len + sp->data_len;
	/*
	char *buffer = (char *)malloc(sizeof(char) * (sp->data_len + 1));
	if (buffer == NULL) {
		return -1;
	}
	*/

	strcpy(header, sp->type);
	memcpy(header + BLOCK_TYPE_SIZE, &(sp->data_len), PACKET_LEN_SIZE);
	sftt_encrypt_func(header, header_len); 
	int ret = send(sock, header, header_len, 0);
	if (ret != header_len) {
		//free(buffer);
		return -1;
	}
	
	sftt_encrypt_func(sp->content, sp->data_len);
	//memcpy(buffer + BLOCK_TYPE_SIZE + PACKET_LEN_SIZE, sp->content, sp->data_len);
	 
	//sftt_encrypt_func(buffer, packet_size); 
	
	ret = send(sock, sp->content, sp->data_len, 0);
	
	//free(buffer);

	if (ret != sp->data_len) {
		return -1;
	}	

	memset(sp->content, 0, sp->block_size);

	return 0;
}

int recv_sftt_packet(int sock, sftt_packet *sp) {
	char header[BLOCK_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 

	int ret = recv(sock, header, header_len, 0);
	if (ret != header_len) {
		return -1;
	}  
	sftt_decrypt_func(header, header_len);

	memcpy(sp->type, header, BLOCK_TYPE_SIZE);
	sp->type[BLOCK_TYPE_SIZE] = 0;
        memcpy(&(sp->data_len), header + BLOCK_TYPE_SIZE, PACKET_LEN_SIZE);
	
	memset(sp->content, 0, sp->block_size);
	ret = recv(sock, sp->content, sp->data_len, 0);
	if (ret != sp->data_len) {
		return -1;
	}	
	sftt_decrypt_func(sp->content, sp->data_len);
	if (strcmp(sp->type, BLOCK_TYPE_FILE_NAME) == 0) {
		printf("decrypted file name: %s\n", sp->content);
		//sleep(1);
	}
		
	return 0;
} 

void free_sftt_packet(sftt_packet **sp) {
	if (sp && *sp) {
		free((*sp)->content);
		free(*sp);
		*sp = NULL;
	} 
}

