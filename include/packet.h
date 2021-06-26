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

#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdint.h>
#include "config.h"
#include "md5.h"
#include "req_rsp.h"
#include "session.h"

#define PACKET_TYPE_SIZE		(sizeof(int))
#define PACKET_LEN_SIZE			(sizeof(int))	

#define REQ_PACKET_MIN_LEN		51200

#define EXTERN_PACKET_MIN_LEN(type) \
	extern int type##_REQ_PACKET_MIN_LEN; \
	extern int type##_RESP_PACKET_MIN_LEN;

EXTERN_PACKET_MIN_LEN(VALIDATE);
EXTERN_PACKET_MIN_LEN(PWD);
EXTERN_PACKET_MIN_LEN(CD);
EXTERN_PACKET_MIN_LEN(LL);
EXTERN_PACKET_MIN_LEN(PUT);
EXTERN_PACKET_MIN_LEN(GET);

enum packet_type {
	PACKET_TYPE_VALIDATE_REQ,
	PACKET_TYPE_VALIDATE_RSP,
	PACKET_TYPE_FILE_NAME_REQ,
	PACKET_TYPE_FILE_NAME_RSP,
	PACKET_TYPE_DATA_REQ,
	PACKET_TYPE_DATA_RSP,
	PACKET_TYPE_FILE_END_REQ,
	PACKET_TYPE_FILE_END_RSP,
	PACKET_TYPE_SEND_COMPLETE_REQ,
	PACKET_TYPE_SEND_COMPLETE_RSP,
	PACKET_TYPE_PWD_REQ,
	PACKET_TYPE_PWD_RSP,
	PACKET_TYPE_CD_REQ,
	PACKET_TYPE_CD_RSP,
	PACKET_TYPE_LL_REQ,
	PACKET_TYPE_LL_RSP,
	PACKET_TYPE_PUT_REQ,
	PACKET_TYPE_PUT_RSP,
	PACKET_TYPE_GET_REQ,
	PACKET_TYPE_GET_RSP
};


struct sftt_packet {
	int type;
	void *obj;
	unsigned char *content;
	int data_len;
	int block_size;
};

#endif
