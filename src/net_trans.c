#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "compress.h"
#include "config.h"
#include "debug.h"
#include "encode.h"
#include "encrypt.h"
#include "log.h"
#include "mem_pool.h"
#include "net_trans.h"
#include "serialize.h"

extern struct mem_pool *g_mp;
extern struct serialize_handle serializables[];

struct sftt_packet *malloc_sftt_packet(int block_size) {

	struct sftt_packet *sp = (struct sftt_packet *)mp_malloc(g_mp, sizeof(struct sftt_packet));
	if (sp == NULL) {
		return NULL;
	}	
	memset(sp, 0, sizeof(*sp));

	sp->content = (unsigned char *)mp_malloc(g_mp, sizeof(unsigned char) * block_size);
	if (sp->content == NULL) {
		free(sp);
		return NULL;
	}
	sp->block_size = block_size;	
	
	return sp;
}

int sftt_packet_encode_content(struct sftt_packet *src, struct sftt_packet *dst) {
	//printf("before encode data_len: %d\n", src->data_len);
	//
 	// 1. compress and encrypt content
	// 2. update content len in header
 	//
	dst->data_len = sftt_buffer_encode(src->content, src->data_len, dst->content, true, true);
	
	return 0;
}

void sftt_packet_send_header(int sock, struct sftt_packet *sp) {
	char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 

	// fill header
	memcpy(header, &(sp->type), PACKET_TYPE_SIZE);
	memcpy(header + PACKET_TYPE_SIZE, &(sp->data_len), PACKET_LEN_SIZE);
	
	unsigned char *buffer = mp_malloc(g_mp, header_len);
	assert(buffer != NULL);

	int encoded_len = sftt_buffer_encode(header, header_len, buffer, false, false);
	assert(encoded_len == header_len);

	int ret = send(sock, buffer, encoded_len, 0);
	assert(ret == encoded_len);

	mp_free(g_mp, buffer);
	add_log(LOG_INFO, "send packet header successfully!");
}

void sftt_packet_send_content(int sock, struct sftt_packet *sp) {
	assert(sp->content != NULL);
	add_log(LOG_INFO, "send packet content, sp->content: %p, sp->data_len: %d", sp->content, sp->data_len);
	int ret = send(sock, sp->content, sp->data_len, 0);
	assert(ret == sp->data_len);
	add_log(LOG_INFO, "send packet content successfully!");
}

bool sftt_packet_serialize(struct sftt_packet *sp)
{
	int i = 0, len = 0;
	unsigned char *buf = NULL;
	bool ret = false;

	for (i = 0; serializables[i].packet_type != -1; ++i) {
		if (sp->type == serializables[i].packet_type) {
			ret = serializables[i].serialize(sp->obj, &buf, &len);
			if (ret && buf) {
				if (len < sp->block_size) {
					memcpy(sp->content, buf, len);
					sp->data_len = len;
				}
				free(buf);
			}
			return ret;
		}
	}

	return false;
}

int sftt_packet_deserialize(struct sftt_packet *sp)
{
	int i = 0;

	for (i = 0; serializables[i].packet_type != -1; ++i) {
		if (sp->type == serializables[i].packet_type) {
			return serializables[i].deserialize(sp->content, sp->data_len, &(sp->obj));
		}
	}

	return false;
}

int send_sftt_packet(int sock, struct sftt_packet *sp) {
	add_log(LOG_INFO, "before send packet, packet size: %d", sp->block_size * 2);
	if (!sftt_packet_serialize(sp)) {
		return -1;
	}
	add_log(LOG_INFO, "before send packet, serialize done!");

	struct sftt_packet *_sp = malloc_sftt_packet(sp->block_size * 2);
	if (_sp == NULL) {
		add_log(LOG_ERROR, "before send packet, malloc failed! size: %d", sp->block_size * 2);
		printf("malloc failed! size: %d\n", sp->block_size * 2);
		return -1;
	}

	_sp->type = sp->type;
	sftt_packet_encode_content(sp, _sp);
	add_log(LOG_INFO, "before send packet, encoded content len: %d", _sp->data_len);

	sftt_packet_send_header(sock, _sp);
	sftt_packet_send_content(sock, _sp);

	free_sftt_packet(&_sp);
	add_log(LOG_INFO, "send packet successfully!");

	return 0;
}

int sftt_packet_decode_content(struct sftt_packet *src, struct sftt_packet *dst) {
	/*
	* 1. decompress and decrypt content
	* 2. update content len in header
	*/
 	dst->data_len = sftt_buffer_decode(src->content, src->data_len, dst->content, true, true);
	add_log(LOG_INFO, "after decode data_len: %d", dst->data_len);

	return 0;
}

int sftt_packet_recv_header(int sock, struct sftt_packet *sp) {
	char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header); 

	add_log(LOG_INFO, "recv header_len: %d", header_len);
	int ret = recv(sock, header, header_len, 0);
	if (ret != header_len) {
		if (ret == 0) {
			return ret;
		}
		return -1;
	}

	unsigned char *buffer = mp_malloc(g_mp, header_len);
	assert(buffer != NULL);

	int decoded_len = sftt_buffer_decode(header, header_len, buffer, false, false);
	assert(decoded_len == header_len);

	memcpy(&(sp->type), buffer, PACKET_TYPE_SIZE);
	add_log(LOG_INFO, "recv packet type: %d", sp->type);
    memcpy(&(sp->data_len), buffer + PACKET_TYPE_SIZE, PACKET_LEN_SIZE);

	mp_free(g_mp, buffer);

	return header_len;
}

int sftt_packet_recv_content(int sock, struct sftt_packet *sp) {
	memset(sp->content, 0, sp->block_size);
	int ret = recv(sock, sp->content, sp->data_len, 0);
	if (ret != sp->data_len) {
		if (ret == 0) {
			return ret;
		}
		return -1;
	}
	add_log(LOG_INFO, "recv content: ret(%d), sp->data_len(%d)", ret, sp->data_len);

	if (sp->type == PACKET_TYPE_FILE_NAME_REQ) {
		add_log(LOG_INFO, "decrypted file name: %s", sp->content);
	}

	return sp->data_len;
}

int recv_sftt_packet(int sock, struct sftt_packet *sp) {
	struct sftt_packet *_sp = malloc_sftt_packet(sp->block_size * 2);
	assert(_sp != NULL);
	add_log(LOG_INFO, "before recv packet!");

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
	add_log(LOG_INFO, "recv packet successfully!");
		
	return sp->data_len;
} 

void free_sftt_packet(struct sftt_packet **sp) {
	if (sp && *sp) {
		mp_free(g_mp, (*sp)->content);
		mp_free(g_mp, *sp);
		*sp = NULL;
	} 
}

