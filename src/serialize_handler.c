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
#include "serialize_handler.h"

struct serialize_handler serializables[] = {
	{PACKET_TYPE_VALIDATE_REQ, validate_req_encode, validate_req_decode},
	{PACKET_TYPE_VALIDATE_RSP, validate_resp_encode, validate_resp_decode},
	{PACKET_TYPE_PWD_REQ, pwd_req_encode, pwd_req_decode},
	{PACKET_TYPE_PWD_RSP, pwd_resp_encode, pwd_resp_decode},
	{PACKET_TYPE_CD_REQ, cd_req_encode, cd_req_decode},
	{PACKET_TYPE_CD_RSP, cd_resp_encode, cd_resp_decode},
	{PACKET_TYPE_LL_REQ, ll_req_encode, ll_req_decode},
	{PACKET_TYPE_LL_RSP, ll_resp_encode, ll_resp_decode},
	{PACKET_TYPE_GET_REQ, get_req_encode, get_req_decode},
	{PACKET_TYPE_GET_RSP, get_resp_encode, get_resp_decode},
	{PACKET_TYPE_PUT_REQ, put_req_encode, put_req_decode},
	{PACKET_TYPE_PUT_RSP, put_resp_encode, put_resp_decode},
	{PACKET_TYPE_COMMON_RSP, common_resp_encode, common_resp_decode},
	{-1, NULL, NULL},
};
