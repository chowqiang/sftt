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
#include "rte_errno.h"
#include "serialize_handler.h"

extern struct mem_pool *g_mp;
extern struct serialize_handler serializables[];

struct sftt_packet *malloc_sftt_packet(void)
{
	DBUG_ENTER(__func__);

	struct sftt_packet *sp = (struct sftt_packet *)mp_malloc(g_mp,
		"sftt_packet_struct", sizeof(struct sftt_packet));
	if (sp == NULL) {
		DBUG_RETURN(NULL);
	}
	memset(sp, 0, sizeof(*sp));

	DBUG_RETURN(sp);
}

int sftt_packet_encode_content(struct sftt_packet *src, struct sftt_packet *dst)
{
	DBUG_ENTER(__func__);

	/*
	 * 1. compress and encrypt content
	 * 2. update content len in header
	 */
	DEBUG((DEBUG_DEBUG, "in|data_len=%d\n", src->data_len));
	dst->data_len = sftt_buffer_encode(src->content, src->data_len,
		&dst->content, true, true);
	DEBUG((DEBUG_DEBUG, "out|data_len=%d\n", dst->data_len));

	DBUG_RETURN(dst->data_len);
}


void sftt_packet_send_header(int sock, struct sftt_packet *sp)
{
	DBUG_ENTER(__func__);

	unsigned char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header);
	int ret, encoded_len;
	unsigned char *buffer;

	DEBUG((DEBUG_DEBUG, "in\n"));
	/* fill header */
	memcpy(header, &(sp->type), PACKET_TYPE_SIZE);
	memcpy(header + PACKET_TYPE_SIZE, &(sp->data_len), PACKET_LEN_SIZE);

	encoded_len = sftt_buffer_encode(header, header_len, &buffer, false, false);
	assert(encoded_len == header_len);

	ret = send(sock, buffer, encoded_len, 0);
	if (ret != encoded_len) {
		DEBUG((DEBUG_ERROR, "send header failed|err=%s\n", strerror(errno)));
	}
	assert(ret == encoded_len);

	mp_free(g_mp, buffer);
	DEBUG((DEBUG_DEBUG, "out\n"));

	DBUG_VOID_RETURN;
}

void sftt_packet_send_content(int sock, struct sftt_packet *sp)
{
	DBUG_ENTER(__func__);

	int ret;

	DEBUG((DEBUG_DEBUG, "in\n"));

	assert(sp->content != NULL);
	DEBUG((DEBUG_DEBUG, "send packet content|sp->content=%p|sp->data_len=%d\n",
		sp->content, sp->data_len));

	ret = send(sock, sp->content, sp->data_len, 0);
	assert(ret == sp->data_len);

	DEBUG((DEBUG_DEBUG, "out\n"));

	DBUG_VOID_RETURN;
}

bool sftt_packet_serialize(struct sftt_packet *sp)
{
	DBUG_ENTER(__func__);

	int i = 0, len = 0;
	unsigned char *buf = NULL;
	bool ret = false;

	DEBUG((DEBUG_DEBUG, "in\n"));
	for (i = 0; serializables[i].packet_type != -1; ++i) {
		if (sp->type == serializables[i].packet_type) {
			ret = serializables[i].serialize(sp->obj, &buf, &len);
			DEBUG((DEBUG_DEBUG, "serialization done|"
					"sp->type=%d|ret=%d|buf=%p|len=%d\n",
					sp->type, ret, buf, len));
			if (ret) {
				assert(buf != NULL);
				sp->content = mp_malloc(g_mp, __func__,
						len * sizeof(unsigned char));
				memcpy(sp->content, buf, len);
				sp->data_len = len;
			} else {
				DEBUG((DEBUG_WARN, "serialization failed|sp->type=%d|"
						"ret=%d|buf=%p|len=%d\n",
						sp->type, ret, buf, len));
			}
			if (buf)
				free(buf);
			DEBUG((DEBUG_DEBUG, "out|ret=%d\n", ret));
			DBUG_RETURN(ret);
		}
	}
	DEBUG((DEBUG_WARN, "unknown packet|type=%d|ret=%d\n", sp->type, ret));

	DBUG_RETURN(false);
}

int sftt_packet_deserialize(struct sftt_packet *sp)
{
	DBUG_ENTER(__func__);

	int i = 0, ret = 0;

	DEBUG((DEBUG_DEBUG, "in\n"));
	for (i = 0; serializables[i].packet_type != -1; ++i) {
		if (sp->type == serializables[i].packet_type) {
			ret = serializables[i].deserialize(sp->content,
				sp->data_len, &(sp->obj));
			DEBUG((DEBUG_DEBUG, "deserialization done|"
					"sp->type=%d|ret=%d|sp->data_len=%d|"
					"sp->obj=%p\n", sp->type, ret,
					sp->data_len, sp->obj));
			if (!ret) {
				DEBUG((DEBUG_WARN, "deserialization failed|"
						"sp->type=%d|ret=%d|"
						"sp->data_len=%d|sp->obj=%p\n",
						sp->type, ret, sp->data_len,
						sp->obj));
			}
			DBUG_RETURN(ret);
		}
	}

	DBUG_RETURN(false);
}

void sftt_packet_free_content(struct sftt_packet *sp)
{
	if (sp == NULL)
	      return;

	if (sp->content)
		mp_free(g_mp, sp->content);

	sp->content = NULL;
	sp->data_len = 0;
}

int send_sftt_packet(int sock, struct sftt_packet *sp)
{
	DBUG_ENTER(__func__);

	int ret = 0;
	struct sftt_packet *_sp;

	DEBUG((DEBUG_DEBUG, "in\n"));
	DEBUG((DEBUG_DEBUG, "before serialize|sp->data_len=%d\n", sp->data_len));

	if (!sftt_packet_serialize(sp)) {
		DEBUG((DEBUG_ERROR, "sftt packet serialize failed!\n"));
		DBUG_RETURN(-1);
	}
	DEBUG((DEBUG_DEBUG, "after serialize|sp->data_len=%d\n", sp->data_len));

	_sp = malloc_sftt_packet();
	if (_sp == NULL) {
		DEBUG((DEBUG_ERROR, "malloc sftt packet failed\n"));
		DBUG_RETURN(-1);
	}

	DEBUG((DEBUG_DEBUG, "before encode content|sp->data_len=%d\n", sp->data_len));

	_sp->type = sp->type;
	ret = sftt_packet_encode_content(sp, _sp);
	if (!(ret > 0)) {
		DEBUG((DEBUG_ERROR, "encode content failed|ret=%d|sp->type=%d\n",
				ret, sp->type));
		DBUG_RETURN(-1);
	}
	sftt_packet_free_content(sp);

	DEBUG((DEBUG_DEBUG, "before send|sp->data_len=%d\n", _sp->data_len));
	sftt_packet_send_header(sock, _sp);
	sftt_packet_send_content(sock, _sp);
	DEBUG((DEBUG_DEBUG, "send packet done\n"));

	free_sftt_packet(&_sp);
	DEBUG((DEBUG_DEBUG, "out\n"));

	DBUG_RETURN(0);
}

int sftt_packet_decode_content(struct sftt_packet *src, struct sftt_packet *dst)
{
	DBUG_ENTER(__func__);

	/*
	* 1. decompress and decrypt content
	* 2. update content len in header
	*/
	DEBUG((DEBUG_DEBUG, "in\n"));
 	dst->data_len = sftt_buffer_decode(src->content, src->data_len,
		&dst->content, true, true);
	DEBUG((DEBUG_DEBUG, "after decode|dst->data_len=%d\n", dst->data_len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	DBUG_RETURN(dst->data_len);
}

int sftt_packet_recv_header(int sock, struct sftt_packet *sp)
{
	DBUG_ENTER(__func__);

	unsigned char header[PACKET_TYPE_SIZE + PACKET_LEN_SIZE];
	int header_len = sizeof(header);
	int decoded_len, ret;
	unsigned char *buffer;

	DEBUG((DEBUG_DEBUG, "in\n"));
	ret = recv(sock, header, header_len, 0);
	rte_errno = errno;
	DEBUG((DEBUG_DEBUG, "recv header|ret=%d|header_len=%d\n",
		ret, header_len));
	if (ret == 0)  {
		DBUG_RETURN(ret);
	}

	if (ret != header_len) {
		DEBUG((DEBUG_WARN, "recv failed because recv ret"
				" not equal to header len|ret=%d|"
				"header_len=%d|err=%s\n", ret, header_len,
				strerror(rte_errno)));
		DBUG_RETURN(-1);
	}

	DEBUG((DEBUG_DEBUG, "before decode header\n"));
	decoded_len = sftt_buffer_decode(header, header_len, &buffer, false, false);
	assert(decoded_len == header_len);

	DEBUG((DEBUG_DEBUG, "header decoded|len=%d\n", decoded_len));

	memcpy(&(sp->type), buffer, PACKET_TYPE_SIZE);
	memcpy(&(sp->data_len), buffer + PACKET_TYPE_SIZE, PACKET_LEN_SIZE);

	DEBUG((DEBUG_DEBUG, "received packet|type=%d\n", sp->type));
	DEBUG((DEBUG_DEBUG, "reported data|len=%d\n", sp->data_len));

	mp_free(g_mp, buffer);
	DEBUG((DEBUG_DEBUG, "out\n"));

	DBUG_RETURN(header_len);
}

int sftt_packet_recv_content(int sock, struct sftt_packet *sp)
{
	DBUG_ENTER(__func__);

	int ret, err;
	DEBUG((DEBUG_DEBUG, "in\n"));

	sp->content = mp_malloc(g_mp, __func__, sp->data_len * sizeof(unsigned char));

	ret = recv(sock, sp->content, sp->data_len, 0 | MSG_WAITALL);
	err = errno;
	DEBUG((DEBUG_DEBUG, "recv content|ret=%d|data_len=%d\n",
		ret, sp->data_len));
	if (ret != sp->data_len) {
		DEBUG((DEBUG_WARN, "recv failed because recv ret"
				" not equal to data len|ret=%d|"
				"data_len=%d|err=%s\n", ret, sp->data_len,
				strerror(err)));
		if (ret == 0) {
			DBUG_RETURN(ret);
		}
		DBUG_RETURN(-1);
	}
	DEBUG((DEBUG_DEBUG, "receive content|ret=%d|sp->data_len=%d\n",
		ret, sp->data_len));

	DBUG_RETURN(sp->data_len);
}

int recv_sftt_packet(int sock, struct sftt_packet *sp)
{
	DBUG_ENTER(__func__);

	struct sftt_packet *_sp = malloc_sftt_packet();
	int ret, recv_len = 0;

	assert(_sp != NULL);
	DEBUG((DEBUG_DEBUG, "in\n"));

	ret = sftt_packet_recv_header(sock, _sp);
	if (!(ret > 0)) {
		DEBUG((DEBUG_ERROR, "recv header failed!\n"));
		DBUG_RETURN(-1);
	}

	recv_len += ret;

	DEBUG((DEBUG_DEBUG, "before receive content\n"));
	ret = sftt_packet_recv_content(sock, _sp);
	if (!(ret > 0)) {
		DEBUG((DEBUG_ERROR, "recv header failed!|ret=%d\n", ret));
		DBUG_RETURN(-1);
	}

	recv_len += ret;

	sp->type = _sp->type;
	DEBUG((DEBUG_DEBUG, "receive packet|type=%d\n", sp->type));
	DEBUG((DEBUG_DEBUG, "before decode content\n"));

	ret = sftt_packet_decode_content(_sp, sp);
	if (!(ret > 0)) {
		DEBUG((DEBUG_ERROR, "decode content failed|ret=%d|sp->type=%d|"
				"recv_len=%d\n", ret, sp->type, recv_len));
		DBUG_RETURN(-1);
	}
	DEBUG((DEBUG_DEBUG, "after decode|sp->data_len=%d\n", sp->data_len));

	DEBUG((DEBUG_DEBUG, "before deserialize\n"));
	if (!sftt_packet_deserialize(sp)) {
		DEBUG((DEBUG_ERROR, "recv deserialize failed!\n"));
		DBUG_RETURN(-1);
	}
	sftt_packet_free_content(sp);

	free_sftt_packet(&_sp);
	DEBUG((DEBUG_DEBUG, "out\n"));

	DBUG_RETURN(recv_len);
}

void free_sftt_packet(struct sftt_packet **sp)
{
	DBUG_ENTER(__func__);

	if (sp && *sp) {
		sftt_packet_free_content(*sp);
		/* Notes!
		 * Don't free sftt_packet obj, let user to free
		 * if ((*sp)->obj)
		 *	mp_free(g_mp, (*sp)->obj);
		 */

		mp_free(g_mp, *sp);
		*sp = NULL;
	}

	DBUG_VOID_RETURN;
}
