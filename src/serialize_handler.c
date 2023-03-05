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

#include "packet.h"
#include "serialize.h"
#include "serialize_raw.h"
#include "serialize_handler.h"

struct serialize_handler serializables[] = {
	{PACKET_TYPE_VALIDATE_REQ, validate_req_encode, validate_req_decode},
	{PACKET_TYPE_VALIDATE_RESP, validate_resp_encode, validate_resp_decode},
	{PACKET_TYPE_PWD_REQ, pwd_req_encode, pwd_req_decode},
	{PACKET_TYPE_PWD_RESP, pwd_resp_encode, pwd_resp_decode},
	{PACKET_TYPE_CD_REQ, cd_req_encode, cd_req_decode},
	{PACKET_TYPE_CD_RESP, cd_resp_encode, cd_resp_decode},
	{PACKET_TYPE_LL_REQ, ll_req_encode, ll_req_decode},
	{PACKET_TYPE_LL_RESP, ll_resp_encode, ll_resp_decode},
	{PACKET_TYPE_GET_REQ, get_req_encode, get_req_decode},
	{PACKET_TYPE_GET_RESP, get_resp_encode, get_resp_decode},
	{PACKET_TYPE_PUT_REQ, put_req_encode, put_req_decode},
	{PACKET_TYPE_PUT_RESP, put_resp_encode, put_resp_decode},
	{PACKET_TYPE_COMMON_RESP, common_resp_encode, common_resp_decode},
	{PACKET_TYPE_WRITE_MSG_REQ, write_msg_req_encode, write_msg_req_decode},
	{PACKET_TYPE_READ_MSG_REQ, read_msg_req_encode, read_msg_req_decode},
	{PACKET_TYPE_READ_MSG_RESP, read_msg_resp_encode, read_msg_resp_decode},
	{PACKET_TYPE_MP_STAT_REQ, mp_stat_req_encode, mp_stat_req_decode},
	{PACKET_TYPE_MP_STAT_RESP, mp_stat_resp_encode, mp_stat_resp_decode},
	{PACKET_TYPE_DIRECTCMD_REQ, directcmd_req_encode, directcmd_req_decode},
	{PACKET_TYPE_DIRECTCMD_RESP, directcmd_resp_encode, directcmd_resp_decode},
	{PACKET_TYPE_WHO_REQ, who_req_encode, who_req_decode},
	{PACKET_TYPE_WHO_RESP, who_resp_encode, who_resp_decode},
	{PACKET_TYPE_WRITE_REQ, write_req_encode, write_req_decode},
	{PACKET_TYPE_WRITE_RESP, write_resp_encode, write_resp_decode},
	{PACKET_TYPE_APPEND_CONN_REQ, append_conn_req_encode, append_conn_req_decode},
	{PACKET_TYPE_APPEND_CONN_RESP, append_conn_resp_encode, append_conn_resp_decode},
	{PACKET_TYPE_CHANNEL_INFO_REQ, channel_info_req_encode, channel_info_req_decode},
	{PACKET_TYPE_CHANNEL_INFO_RESP, channel_info_resp_encode, channel_info_resp_decode},
	{PACKET_TYPE_PORT_UPDATE_REQ, port_update_req_encode, port_update_req_decode},
	{PACKET_TYPE_PORT_UPDATE_RESP, port_update_resp_encode, port_update_resp_decode},
	{PACKET_TYPE_RECONNECT_REQ, reconnect_req_encode, reconnect_req_decode},
	{PACKET_TYPE_RECONNECT_RESP, reconnect_resp_encode, reconnect_resp_decode},
	{-1, NULL, NULL},
};

struct serialize_handler serializables_raw[] = {
	{PACKET_TYPE_VALIDATE_REQ, validate_req_raw_encode, validate_req_raw_decode},
	{PACKET_TYPE_VALIDATE_RESP, validate_resp_raw_encode, validate_resp_raw_decode},
	{PACKET_TYPE_PWD_REQ, pwd_req_raw_encode, pwd_req_raw_decode},
	{PACKET_TYPE_PWD_RESP, pwd_resp_raw_encode, pwd_resp_raw_decode},
	{PACKET_TYPE_CD_REQ, cd_req_raw_encode, cd_req_raw_decode},
	{PACKET_TYPE_CD_RESP, cd_resp_raw_encode, cd_resp_raw_decode},
	{PACKET_TYPE_LL_REQ, ll_req_raw_encode, ll_req_raw_decode},
	{PACKET_TYPE_LL_RESP, ll_resp_raw_encode, ll_resp_raw_decode},
	{PACKET_TYPE_GET_REQ, get_req_raw_encode, get_req_raw_decode},
	{PACKET_TYPE_GET_RESP, get_resp_raw_encode, get_resp_raw_decode},
	{PACKET_TYPE_PUT_REQ, put_req_raw_encode, put_req_raw_decode},
	{PACKET_TYPE_PUT_RESP, put_resp_raw_encode, put_resp_raw_decode},
	{PACKET_TYPE_COMMON_RESP, common_resp_raw_encode, common_resp_raw_decode},
	{PACKET_TYPE_WRITE_MSG_REQ, write_msg_req_raw_encode, write_msg_req_raw_decode},
	{PACKET_TYPE_READ_MSG_REQ, read_msg_req_raw_encode, read_msg_req_raw_decode},
	{PACKET_TYPE_READ_MSG_RESP, read_msg_resp_raw_encode, read_msg_resp_raw_decode},
	{PACKET_TYPE_MP_STAT_REQ, mp_stat_req_raw_encode, mp_stat_req_raw_decode},
	{PACKET_TYPE_MP_STAT_RESP, mp_stat_resp_raw_encode, mp_stat_resp_raw_decode},
	{PACKET_TYPE_DIRECTCMD_REQ, directcmd_req_raw_encode, directcmd_req_raw_decode},
	{PACKET_TYPE_DIRECTCMD_RESP, directcmd_resp_raw_encode, directcmd_resp_raw_decode},
	{PACKET_TYPE_WHO_REQ, who_req_raw_encode, who_req_raw_decode},
	{PACKET_TYPE_WHO_RESP, who_resp_raw_encode, who_resp_raw_decode},
	{PACKET_TYPE_WRITE_REQ, write_req_raw_encode, write_req_raw_decode},
	{PACKET_TYPE_WRITE_RESP, write_resp_raw_encode, write_resp_raw_decode},
	{PACKET_TYPE_APPEND_CONN_REQ, append_conn_req_raw_encode, append_conn_req_raw_decode},
	{PACKET_TYPE_APPEND_CONN_RESP, append_conn_resp_raw_encode, append_conn_resp_raw_decode},
	{PACKET_TYPE_CHANNEL_INFO_REQ, channel_info_req_raw_encode, channel_info_req_raw_decode},
	{PACKET_TYPE_CHANNEL_INFO_RESP, channel_info_resp_raw_encode, channel_info_resp_raw_decode},
	{PACKET_TYPE_PORT_UPDATE_REQ, port_update_req_raw_encode, port_update_req_raw_decode},
	{PACKET_TYPE_PORT_UPDATE_RESP, port_update_resp_raw_encode, port_update_resp_raw_decode},
	{PACKET_TYPE_RECONNECT_REQ, reconnect_req_raw_encode, reconnect_req_raw_decode},
	{PACKET_TYPE_RECONNECT_RESP, reconnect_resp_raw_encode, reconnect_resp_raw_decode},
	{-1, NULL, NULL},
};
