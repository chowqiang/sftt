#include <stdlib.h>
#if __linux__ 
#include <malloc.h>
#endif
#include <sys/socket.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "net_trans.h"
#include "config.h"
#include "encrypt.h"
#include "memory_pool.h"
#include "compress.h"

sftt_packet *malloc_sftt_packet(int block_size) {
	memory_pool *mp = get_singleton_mp();

	sftt_packet *sp = (sftt_packet *)mp_malloc(mp, sizeof(sftt_packet));
	if (sp == NULL) {
		return NULL;
	}	
	memset(sp, 0, sizeof(*sp));

	sp->content = (unsigned char *)mp_malloc(mp, sizeof(unsigned char) * block_size);
	if (sp->content == NULL) {
		free(sp);
		return NULL;
	}
	sp->block_size = block_size;	
	
	return sp;
}

int sftt_packet_encode(sftt_packet *src, sftt_packet *dst) {
	//
 	// 1. compress and encrypt content
	// 2. update content len in header
 	//
	dst->data_len = sftt_buffer_encode(src->content, src->data_len, dst->content, true, true);
	
	return 0;
}

void sftt_packet_send_header(int sock, sftt_packet *sp) {
	char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 

	// fill header
	// strcpy(header, sp->type);
	memcpy(header, &(sp->type), PACKET_TYPE_SIZE);
	memcpy(header + PACKET_TYPE_SIZE, &(sp->data_len), PACKET_LEN_SIZE);
	
	memory_pool *mp = get_singleton_mp();
	unsigned char *buffer = mp_malloc(mp, header_len);

	int encoded_len = sftt_buffer_encode(header, header_len, buffer, false, true); 
	int ret = send(sock, buffer, encoded_len, 0);

	mp_free(mp, buffer);
}

void sftt_packet_send_content(int sock, sftt_packet *sp) {
	int ret = send(sock, sp->content, sp->data_len, 0);
}

int send_sftt_packet(int sock, sftt_packet *sp) {
	sftt_packet *_sp = malloc_sftt_packet(sp->block_size * 2);

	sftt_packet_encode(sp, _sp);

	sftt_packet_send_header(sock, _sp);
	sftt_packet_send_content(sock, _sp);

	free_sftt_packet(&_sp);

	return 0;
}

int sftt_packet_decode(sftt_packet *src, sftt_packet *dst) {
	//
 	// 1. decompress and decrypt content
	// 2. update content len in header
 	//
 	dst->data_len = sftt_buffer_decode(src->content, src->data_len, dst->content, true, true);

	return 0;
}

int sftt_packet_recv_header(int sock, sftt_packet *sp) {
	char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 

	int ret = recv(sock, header, header_len, 0);
	assert(ret == header_len);

	memory_pool *mp = get_singleton_mp();
	unsigned char *buffer = mp_malloc(mp, header_len);

	int decoded_len = sftt_buffer_decode(header, header_len, buffer, false, true);
	assert(decoded_len == header_len);

	memcpy(&(sp->type), buffer, PACKET_TYPE_SIZE);
	// sp->type[PACKET_TYPE_SIZE] = 0;
    memcpy(&(sp->data_len), buffer + PACKET_TYPE_SIZE, PACKET_LEN_SIZE);

	mp_free(mp, buffer);

	return 0;
}

int sftt_packet_recv_content(int sock, sftt_packet *sp) {
	memset(sp->content, 0, sp->block_size);
	int ret = recv(sock, sp->content, sp->data_len, 0);
	assert(ret == sp->data_len);

	if (sp->type == PACKET_TYPE_FILE_NAME) {
		printf("decrypted file name: %s\n", sp->content);
	}

	return 0;
}

int recv_sftt_packet(int sock, sftt_packet *sp) {
	sftt_packet *_sp = malloc_sftt_packet(sp->block_size * 2); 

	sftt_packet_recv_header(sock, sp);
	sftt_packet_recv_content(sock, sp);

	sftt_packet_decode(_sp, sp);

	free_sftt_packet(&_sp);	
		
	return 0;
} 

void free_sftt_packet(sftt_packet **sp) {
	memory_pool *mp = get_singleton_mp();

	if (sp && *sp) {
		mp_free(mp, (*sp)->content);
		mp_free(mp, *sp);
		*sp = NULL;
	} 
}

