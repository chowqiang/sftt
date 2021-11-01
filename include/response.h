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

#ifndef _RESPONSE_H_
#define _RESPONSE_H_

enum resp_code {
	RESP_OK,
	RESP_SERVER_EXCEPTION,
	RESP_CANNOT_CD,
	RESP_CONTINUE,
	RESP_UVS_NTFD,
	RESP_UVS_INVALID,
	RESP_UVS_MISSHOME,
	RESP_UVS_BLOCK,
	RESP_UVS_BAD_VER,
	RESP_UVS_PASS,
	RESP_CNT_CHECK_USER,
	RESP_CNT_GET_PEER,
	RESP_SEND_PEER_ERR,
	RESP_SESSION_INVALID,
	RESP_FILE_NTFD,
	RESP_INTERNAL_ERR,
	RESP_PATH_NOT_ABS,
	RESP_UNKNOWN_FILE_TYPE,
	RESP_UNKNOWN_CONN_TYPE,
	RESP_CNT_GET_TASK_CONN,
};

#endif
