/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
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
#include "serialize_handler.h"

extern struct mem_pool *g_mp;
extern struct serialize_handler serializables[];

struct sftt_packet *malloc_sftt_packet(void)
{
	struct sftt_packet *sp = (struct sftt_packet *)mp_malloc(g_mp,
		"sftt_packet_struct", sizeof(struct sftt_packet));
	if (sp == NULL) {
		return NULL;
	}	
	memset(sp, 0, sizeof(*sp));

	return sp;
}

int sftt_packet_encode_content(struct sftt_packet *src, struct sftt_packet *dst)
{
	/*
	 * 1. compress and encrypt content
	 * 2. update content len in header
	 */
	add_log(LOG_INFO, "%s: in, data_len=%d", __func__, src->data_len);
	dst->data_len = sftt_buffer_encode(src->content, src->data_len,
		&dst->content, true, true);
	add_log(LOG_INFO, "%s: out, data_len=%d", __func__, dst->data_len);
	
	return 0;
}


void sftt_packet_send_header(int sock, struct sftt_packet *sp)
{
	unsigned char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header);
	int ret, encoded_len;
	unsigned char *buffer;

	add_log(LOG_INFO, "%s: in", __func__);
	/* fill header */
	memcpy(header, &(sp->type), PACKET_TYPE_SIZE);
	memcpy(header + PACKET_TYPE_SIZE, &(sp->data_len), PACKET_LEN_SIZE);
	
#if 0
	buffer = mp_malloc(g_mp, __func__, header_len);
	assert(buffer != NULL);
#endif

	encoded_len = sftt_buffer_encode(header, header_len, &buffer, false, false);
	assert(encoded_len == header_len);

	ret = send(sock, buffer, encoded_len, 0);
	if (ret != encoded_len) {
		printf("send header failed! %s\n", strerror(errno));
	}
	assert(ret == encoded_len);

	mp_free(g_mp, buffer);
	add_log(LOG_INFO, "%s: out", __func__);
}

void sftt_packet_send_content(int sock, struct sftt_packet *sp)
{
	int ret;

	add_log(LOG_INFO, "%s: in", __func__);

	assert(sp->content != NULL);
	add_log(LOG_INFO, "send packet content, sp->content=%p, sp->data_len=%d",
		sp->content, sp->data_len);

	ret = send(sock, sp->content, sp->data_len, 0);
	assert(ret == sp->data_len);

	add_log(LOG_INFO, "%s: out", __func__);
}

bool sftt_packet_serialize(struct sftt_packet *sp)
{
	int i = 0, len = 0;
	unsigned char *buf = NULL;
	bool ret = false;

	add_log(LOG_INFO, "%s: in", __func__);
	for (i = 0; serializables[i].packet_type != -1; ++i) {
		if (sp->type == serializables[i].packet_type) {
			ret = serializables[i].serialize(sp->obj, &buf, &len);
			if (ret && buf) {
				sp->content = mp_malloc(g_mp, __func__,
						len * sizeof(unsigned char));
				memcpy(sp->content, buf, len);
				sp->data_len = len;

				free(buf);
			}
			add_log(LOG_INFO, "%s: out, ret=%d", __func__, ret);
			return ret;
		}
	}
	add_log(LOG_INFO, "%s: out, ret=%d", __func__, ret);

	return false;
}

int sftt_packet_deserialize(struct sftt_packet *sp)
{
	int i = 0, ret = 0;

	add_log(LOG_INFO, "%s: in", __func__);
	for (i = 0; serializables[i].packet_type != -1; ++i) {
		if (sp->type == serializables[i].packet_type) {
			ret = serializables[i].deserialize(sp->content,
				sp->data_len, &(sp->obj));
			add_log(LOG_INFO, "%s: out, ret=%d", __func__, ret);
			return ret;
		}
	}

	return false;
}

int send_sftt_packet(int sock, struct sftt_packet *sp)
{
	struct sftt_packet *_sp;

	add_log(LOG_INFO, "%s: in", __func__);
	add_log(LOG_INFO, "%s: before serialize, packet data_len=%d", __func__,
		sp->data_len);

	if (!sftt_packet_serialize(sp)) {
		printf("sftt packet serialize failed!\n");
		return -1;
	}
	add_log(LOG_INFO, "%s: after serialize, packet data_len=%d", __func__,
		sp->data_len);

	_sp = malloc_sftt_packet();
	if (_sp == NULL) {
		printf("%s:%d, malloc sftt packet failed!\n", __func__, __LINE__);
		return -1;
	}

	add_log(LOG_INFO, "%s: before encode content, packet data_len=%d", __func__,
		sp->data_len);

	_sp->type = sp->type;
	sftt_packet_encode_content(sp, _sp);

	add_log(LOG_INFO, "%s: before send, data_len=%d", __func__, _sp->data_len);
	sftt_packet_send_header(sock, _sp);
	sftt_packet_send_content(sock, _sp);
	add_log(LOG_INFO, "%s: send packet done!", __func__);

	free_sftt_packet(&_sp);
	add_log(LOG_INFO, "%s: out", __func__);

	return 0;
}

int sftt_packet_decode_content(struct sftt_packet *src, struct sftt_packet *dst)
{
	/*
	* 1. decompress and decrypt content
	* 2. update content len in header
	*/
	add_log(LOG_INFO, "%s: in", __func__);
 	dst->data_len = sftt_buffer_decode(src->content, src->data_len,
		&dst->content, true, true);
	add_log(LOG_INFO, "%s: after decode, data_len=%d", __func__, dst->data_len);
	add_log(LOG_INFO, "%s: out", __func__);

	return 0;
}

int sftt_packet_recv_header(int sock, struct sftt_packet *sp)
{
	unsigned char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header);
	int decoded_len, ret;
	unsigned char *buffer;
	int err;

	add_log(LOG_INFO, "%s: in", __func__);
	ret = recv(sock, header, header_len, 0);
	err = errno;
	add_log(LOG_INFO, "%s: recv header, ret=%d, header_len=%d", __func__,
		ret, header_len);
	if (ret != header_len) {
		DEBUG((DEBUG_WARN, "recv failed: %s\n", strerror(err)));
		DEBUG((DEBUG_WARN, "recv ret not equal to header len, "
				"ret = %d, header_len = %d\n", ret, header_len));
		if (ret == 0) {
			DEBUG((DEBUG_WARN, "received zero byte!\n"));
			return ret;
		}
		return -1;
	}

#if 0
	buffer = mp_malloc(g_mp, __func__, header_len);
	assert(buffer != NULL);
#endif

	add_log(LOG_INFO, "%s: before decode header", __func__);
	decoded_len = sftt_buffer_decode(header, header_len, &buffer, false, false);
	assert(decoded_len == header_len);

	add_log(LOG_INFO, "%s: header decoded len: %d", __func__, decoded_len);

	memcpy(&(sp->type), buffer, PACKET_TYPE_SIZE);
	memcpy(&(sp->data_len), buffer + PACKET_TYPE_SIZE, PACKET_LEN_SIZE);

	add_log(LOG_INFO, "%s: received packet type: %d", __func__, sp->type);
	add_log(LOG_INFO, "%s: reported data len: %d", __func__, sp->data_len);

	mp_free(g_mp, buffer);
	add_log(LOG_INFO, "%s: out", __func__);

	return header_len;
}

int sftt_packet_recv_content(int sock, struct sftt_packet *sp)
{
	int ret;
	add_log(LOG_INFO, "%s: in", __func__);

	sp->content = mp_malloc(g_mp, __func__, sp->data_len * sizeof(unsigned char));

	ret = recv(sock, sp->content, sp->data_len, 0 | MSG_WAITALL);
	if (ret != sp->data_len) {
		printf("%s: receive result not equal to data len, ret=%d, data_len=%d\n",
			__func__, ret, sp->data_len);
		if (ret == 0) {
			return ret;
		}
		return -1;
	}
	add_log(LOG_INFO, "%s: receive content, ret=%d, sp->data_len=%d", __func__,
		ret, sp->data_len);

#if 0
	if (sp->type == PACKET_TYPE_FILE_NAME_REQ) {
		add_log(LOG_INFO, "decrypted file name: %s", sp->content);
	}
#endif

	return sp->data_len;
}

int recv_sftt_packet(int sock, struct sftt_packet *sp)
{
	struct sftt_packet *_sp = malloc_sftt_packet();
	int ret, recv_len = 0;

	assert(_sp != NULL);
	add_log(LOG_INFO, "%s: in", __func__);

	ret = sftt_packet_recv_header(sock, _sp);
	if (!(ret > 0)) {
		DEBUG((DEBUG_WARN, "%s: recv header failed!\n", __func__));
		return ret;
	}

	recv_len += ret;

	add_log(LOG_INFO, "%s: before receive content", __func__);
	ret = sftt_packet_recv_content(sock, _sp);
	if (!(ret > 0)) {
		printf("%s: recv content failed!\n", __func__);
		return ret;
	}

	recv_len += ret;

	sp->type = _sp->type;
	add_log(LOG_INFO, "%s: receive packet type: %d", __func__, sp->type);
	add_log(LOG_INFO, "%s: before decode content", __func__);

	sftt_packet_decode_content(_sp, sp);
	DEBUG((DEBUG_INFO, "after decode|sp->data_len=%d\n", sp->data_len));

	add_log(LOG_INFO, "%s: before deserialize", __func__);
	if (!sftt_packet_deserialize(sp)) {
		printf("%s: recv deserialize failed!\n", __func__);
	}

	free_sftt_packet(&_sp);	
	add_log(LOG_INFO, "%s: out", __func__);
		
	return recv_len;
}

void free_sftt_packet(struct sftt_packet **sp)
{
	if (sp && *sp) {
		if ((*sp)->content) {
			mp_free(g_mp, (*sp)->content);
			(*sp)->content = NULL;
		}
#if 0
		mp_free(g_mp, (*sp)->content);
#endif
		mp_free(g_mp, *sp);
#if 0
		if ((*sp)->obj)
			mp_free(g_mp, (*sp)->obj);
#endif
		*sp = NULL;
	}
}

