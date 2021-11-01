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

#include "req_resp.h"

#define SET_REQ_PACKET_MIN_LEN(type, ltype) \
	int type##_REQ_PACKET_MIN_LEN = (sizeof(struct ltype##_req) * 20);

#define SET_RESP_PACKET_MIN_LEN(type, ltype) \
	int type##_RESP_PACKET_MIN_LEN = (sizeof(struct ltype##_resp) * 20);

SET_REQ_PACKET_MIN_LEN(VALIDATE, validate)
SET_RESP_PACKET_MIN_LEN(VALIDATE, validate)

SET_REQ_PACKET_MIN_LEN(PWD, pwd)
SET_RESP_PACKET_MIN_LEN(PWD, pwd)

SET_REQ_PACKET_MIN_LEN(CD, cd)
SET_RESP_PACKET_MIN_LEN(CD, cd)

SET_REQ_PACKET_MIN_LEN(LL, ll)
SET_RESP_PACKET_MIN_LEN(LL, ll)

SET_REQ_PACKET_MIN_LEN(PUT, put)
SET_RESP_PACKET_MIN_LEN(PUT, put)

SET_REQ_PACKET_MIN_LEN(GET, get)
SET_RESP_PACKET_MIN_LEN(GET, get)

SET_RESP_PACKET_MIN_LEN(COMMON, common)

SET_REQ_PACKET_MIN_LEN(WRITE_MSG, write_msg)

SET_REQ_PACKET_MIN_LEN(READ_MSG, read_msg)
SET_RESP_PACKET_MIN_LEN(READ_MSG, read_msg)

SET_REQ_PACKET_MIN_LEN(MP_STAT, mp_stat)
SET_RESP_PACKET_MIN_LEN(MP_STAT, mp_stat)

SET_REQ_PACKET_MIN_LEN(DIRECTCMD, directcmd)
SET_RESP_PACKET_MIN_LEN(DIRECTCMD, directcmd)

SET_REQ_PACKET_MIN_LEN(WHO, who)
SET_RESP_PACKET_MIN_LEN(WHO, who)

SET_REQ_PACKET_MIN_LEN(WRITE, write)
SET_RESP_PACKET_MIN_LEN(WRITE, write)

SET_REQ_PACKET_MIN_LEN(APPEND_CONN, append_conn)
SET_RESP_PACKET_MIN_LEN(APPEND_CONN, append_conn)
