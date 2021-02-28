#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "compress.h"
#include "config.h"
#include "debug.h"
#include "encrypt.h"
#include "mem_pool.h"
#include "net_trans.h"
#include "serialize.h"

extern struct serialize_handle *serializables;

sftt_packet *malloc_sftt_packet(int block_size) {
	mem_pool *mp = get_singleton_mp();

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

int sftt_packet_encode_content(sftt_packet *src, sftt_packet *dst) {
	//printf("before encode data_len: %d\n", src->data_len);
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
	
	mem_pool *mp = get_singleton_mp();
	assert(mp != NULL);
	unsigned char *buffer = mp_malloc(mp, header_len);
	assert(buffer != NULL);

	int encoded_len = sftt_buffer_encode(header, header_len, buffer, false, false);
	assert(encoded_len == header_len);

	int ret = send(sock, buffer, encoded_len, 0);
	assert(ret == encoded_len);

	//DEBUG_POINT;
	mp_free(mp, buffer);
	//DEBUG_POINT;
}

void sftt_packet_send_content(int sock, sftt_packet *sp) {
	//DEBUG_POINT;
	assert(sp->content != NULL);
	//printf("sp->content: %p, sp->data_len: %d\n", sp->content, sp->data_len);
	//DEBUG_POINT;
	int ret = send(sock, sp->content, sp->data_len, 0);
	//DEBUG_POINT;
	assert(ret == sp->data_len);
}

bool sftt_packet_serialize(sftt_packet *sp)
{
	int i = 0;

	for (i = 0; serializables[i].packet_type != -1; ++i) {
		if (sp->type == serializables[i].packet_type) {
			return (*serializables[i].serialize)(sp->obj, &(sp->content), &(sp->data_len));
		}
	}

	return false;
}

int sftt_packet_deserialize(sftt_packet *sp)
{
	int i = 0;

	for (i = 0; serializables[i].packet_type != -1; ++i) {
		if (sp->type == serializables[i].packet_type) {
			return (*serializables[i].deserialize)(sp->content, sp->data_len, sp->obj);
		}
	}

	return false;
}

int send_sftt_packet(int sock, sftt_packet *sp) {
	if (!sftt_packet_serialize(sp)) {
		return -1;
	}

	sftt_packet *_sp = malloc_sftt_packet(sp->block_size * 2);

	_sp->type = sp->type;
	sftt_packet_encode_content(sp, _sp);
	//printf("encoded content len: %d\n", _sp->data_len);

	sftt_packet_send_header(sock, _sp);
	sftt_packet_send_content(sock, _sp);

	free_sftt_packet(&_sp);
	//DEBUG_POINT;

	return 0;
}

int sftt_packet_decode_content(sftt_packet *src, sftt_packet *dst) {
	//
 	// 1. decompress and decrypt content
	// 2. update content len in header
 	//
 	dst->data_len = sftt_buffer_decode(src->content, src->data_len, dst->content, true, true);
	//printf("after decode data_len: %d\n", dst->data_len);

	return 0;
}

int sftt_packet_recv_header(int sock, sftt_packet *sp) {
	char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 

	//DEBUG_POINT;
	int ret = recv(sock, header, header_len, 0);
	if (ret != header_len) {
		if (ret == 0) {
			return ret;
		}
		return -1;
	}

	mem_pool *mp = get_singleton_mp();
	assert(mp != NULL);
	unsigned char *buffer = mp_malloc(mp, header_len);
	assert(buffer != NULL);

	int decoded_len = sftt_buffer_decode(header, header_len, buffer, false, false);
	assert(decoded_len == header_len);

	memcpy(&(sp->type), buffer, PACKET_TYPE_SIZE);
	//printf("recv packet type: %d\n", sp->type);
    memcpy(&(sp->data_len), buffer + PACKET_TYPE_SIZE, PACKET_LEN_SIZE);

	mp_free(mp, buffer);

	return header_len;
}

int sftt_packet_recv_content(int sock, sftt_packet *sp) {
	memset(sp->content, 0, sp->block_size);
	int ret = recv(sock, sp->content, sp->data_len, 0);
	if (ret != sp->data_len) {
		if (ret == 0) {
			return ret;
		}
		return -1;
	}
	//printf("recv content: ret(%d), sp->data_len(%d)\n", ret, sp->data_len);
	//DEBUG_ASSERT(ret == sp->data_len, "ret: %d, sp->data_len: %d\n", ret, sp->data_len);

	if (sp->type == PACKET_TYPE_FILE_NAME_REQ) {
		printf("decrypted file name: %s\n", sp->content);
	}

	return sp->data_len;
}

int recv_sftt_packet(int sock, sftt_packet *sp) {
	sftt_packet *_sp = malloc_sftt_packet(sp->block_size * 2); 
	assert(_sp != NULL);

	int ret = sftt_packet_recv_header(sock, _sp);
	if (!(ret > 0)) {
		return ret;
	}

	ret = sftt_packet_recv_content(sock, _sp);
	if (!(ret > 0)) {
		return ret;
	}

	sp->type = _sp->type;
	sftt_packet_decode_content(_sp, sp);

	sftt_packet_deserialize(sp);
	free_sftt_packet(&_sp);	
		
	return sp->data_len;
} 

void free_sftt_packet(sftt_packet **sp) {
	mem_pool *mp = get_singleton_mp();

	if (sp && *sp) {
		mp_free(mp, (*sp)->content);
		mp_free(mp, *sp);
		*sp = NULL;
	} 
}

