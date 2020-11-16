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
	sftt_packet *sp = (sftt_packet *)malloc(sizeof(sftt_packet));
	if (sp == NULL) {
		return NULL;
	}	
	memset(sp, 0, sizeof(*sp));

	sp->content = (unsigned char *)malloc(sizeof(unsigned char) * block_size);
	if (sp->content == NULL) {
		free(sp);
		return NULL;
	}
	sp->block_size = block_size;	
	
	return sp;
}

int sftt_packet_encode(unsigned char *input, int len, unsigned char *output, bool zip, bool crypt) {
	if (!zip && !crypt) {
		memcpy(output, input, len);
		return len;
	}

	int zip_len = len;
	if (zip) {
		zip_len = huffman_compress(input, len, output);
	}

	if (crypt) {
		sftt_encrypt_func(output, zip_len);
	}

	return zip_len;
}

void sftt_packet_send_header(int sock, sftt_packet *sp) {
	char header[BLOCK_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 

	// fill header
	strcpy(header, sp->type);
	memcpy(header + BLOCK_TYPE_SIZE, &(sp->data_len), PACKET_LEN_SIZE);

	memory_pool *mp = get_singleton_mp();
	unsigned char *buffer = mp_malloc(mp, header_len);

	int encoded_len = sftt_packet_encode(header, header_len, buffer, false, true);
	int ret = send(sock, buffer, encoded_len, 0);

	mp_free(mp, buffer);
}

void sftt_packet_send_content(int sock, sftt_packet *sp) {
	memory_pool *mp = get_singleton_mp();
	unsigned char *buffer = mp_malloc(mp, HUFFMAN_META_DATA_SIZE + sp->data_len);

	int encoded_len = sftt_packet_encode(sp->content, sp->data_len, buffer, true, true);
	int ret = send(sock, buffer, encoded_len, 0);

	mp_free(mp, buffer);
}

int send_sftt_packet(int sock, sftt_packet *sp) {
	sftt_packet_send_header(sock, sp);
	sftt_packet_send_content(sock, sp);

	return 0;
}

int sftt_packet_decode(unsigned char *input, int len, unsigned char *output, bool unzip, bool decrypt) {
	if (!unzip && !decrypt) {
		memcpy(output, input, len);
		return len;
	}

	int unzip_len = len;
	if (unzip) {
		unzip_len = huffman_decompress(input, output);
	}

	if (decrypt) {
		sftt_decrypt_func(output, unzip_len);
	}

	return unzip_len;
}

int sftt_packet_recv_header(int sock, sftt_packet *sp) {
	char header[BLOCK_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 

	int ret = recv(sock, header, header_len, 0);
	assert(ret == header_len);

	memory_pool *mp = get_singleton_mp();
	unsigned char *buffer = mp_malloc(mp, header_len);

	int decoded_len = sftt_packet_decode(header, header_len, buffer, false, true);
	assert(decoded_len == header_len);

	memcpy(sp->type, buffer, BLOCK_TYPE_SIZE);
	sp->type[BLOCK_TYPE_SIZE] = 0;
        memcpy(&(sp->data_len), buffer + BLOCK_TYPE_SIZE, PACKET_LEN_SIZE);

	mp_free(mp, buffer);
}

int sftt_packet_recv_content(int sock, sftt_packet *sp) {
	memset(sp->content, 0, sp->block_size);
	int ret = recv(sock, sp->content, sp->data_len, 0);
	assert(ret == sp->data_len);

	memory_pool *mp = get_singleton_mp();
	unsigned char *buffer = mp_malloc(mp, HUFFMAN_META_DATA_SIZE + sp->data_len * 4);

	int decoded_len = sftt_packet_decode(sp->content, sp->data_len, buffer, true, true);
	memcpy(sp->content, buffer, decoded_len);
	sp->data_len = decoded_len;

	if (strcmp(sp->type, BLOCK_TYPE_FILE_NAME) == 0) {
		printf("decrypted file name: %s\n", sp->content);
	}

	mp_free(mp, buffer);
}

int recv_sftt_packet(int sock, sftt_packet *sp) {
	sftt_packet_recv_header(sock, sp);
	sftt_packet_recv_content(sock, sp);
		
	return 0;
} 

void free_sftt_packet(sftt_packet **sp) {
	if (sp && *sp) {
		free((*sp)->content);
		free(*sp);
		*sp = NULL;
	} 
}

