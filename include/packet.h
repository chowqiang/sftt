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
#include "req_resp.h"
#include "session.h"

#define PACKET_TYPE_SIZE		(sizeof(int))
#define PACKET_LEN_SIZE			(sizeof(int))	

/*
 * The packet type used to send and recv packet.
 */
enum packet_type {
	PACKET_TYPE_VALIDATE_REQ,
	PACKET_TYPE_VALIDATE_RESP,
	PACKET_TYPE_FILE_NAME_REQ,
	PACKET_TYPE_FILE_NAME_RESP,
	PACKET_TYPE_DATA_REQ,
	PACKET_TYPE_DATA_RESP,
	PACKET_TYPE_FILE_END_REQ,
	PACKET_TYPE_FILE_END_RESP,
	PACKET_TYPE_SEND_COMPLETE_REQ,
	PACKET_TYPE_SEND_COMPLETE_RESP,
	PACKET_TYPE_PWD_REQ,
	PACKET_TYPE_PWD_RESP,
	PACKET_TYPE_CD_REQ,
	PACKET_TYPE_CD_RESP,
	PACKET_TYPE_LL_REQ,
	PACKET_TYPE_LL_RESP,
	PACKET_TYPE_PUT_REQ,
	PACKET_TYPE_PUT_RESP,
	PACKET_TYPE_GET_REQ,
	PACKET_TYPE_GET_RESP,
	PACKET_TYPE_COMMON_RESP,
	PACKET_TYPE_WRITE_MSG_REQ,
	PACKET_TYPE_READ_MSG_REQ,
	PACKET_TYPE_READ_MSG_RESP,
	PACKET_TYPE_MP_STAT_REQ,
	PACKET_TYPE_MP_STAT_RESP,
	PACKET_TYPE_DIRECTCMD_REQ,
	PACKET_TYPE_DIRECTCMD_RESP,
	PACKET_TYPE_WHO_REQ,
	PACKET_TYPE_WHO_RESP,
	PACKET_TYPE_WRITE_REQ,
	PACKET_TYPE_WRITE_RESP,
	PACKET_TYPE_APPEND_CONN_REQ,
	PACKET_TYPE_APPEND_CONN_RESP,
	PACKET_TYPE_CHANNEL_INFO_REQ,
	PACKET_TYPE_CHANNEL_INFO_RESP,
	PACKET_TYPE_PORT_UPDATE_REQ,
	PACKET_TYPE_PORT_UPDATE_RESP,
	PACKET_TYPE_RECONNECT_REQ,
	PACKET_TYPE_RECONNECT_RESP,
};

/*
 * The packet struct to describe a packet.
 * @obj: the pointer of struct which will be (de)serialized
 * 	according to the packet type.
 */
struct sftt_packet {
	int type;
	void *obj;
	unsigned char *content;
	int data_len;
};

#endif
