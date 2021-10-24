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

#include "response.h"

const char *resp_messages[] = {
	[RESP_OK] = "ok!",
	[RESP_SERVER_EXCEPTION] = "server exception!",
	[RESP_CANNOT_CD] = "cannot change directory!",
	[RESP_CONTINUE] = "continue!",
	[RESP_UVS_NTFD] = "user not found!",
	[RESP_UVS_INVALID] = "cannot validate user!",
	[RESP_UVS_MISSHOME] = "cannot find user's home!",
	[RESP_UVS_BLOCK] = "user blocked!",
	[RESP_UVS_BAD_VER] = "version mismatch!",
	[RESP_UVS_PASS] = "pass validate!",
	[RESP_CNT_CHECK_USER] = "cannot check user!",
	[RESP_CNT_GET_PEER] = "cannot get peer info!",
	[RESP_SEND_PEER_ERR] = "send to peer failed!",
	[RESP_SESSION_INVALID] = "session invalid!",
	[RESP_FILE_NTFD] = "file not found!",
	[RESP_INTERNAL_ERR] = "internal error!",
	[RESP_PATH_NOT_ABS] = "path not absolute!",
	[RESP_UNKNOWN_FILE_TYPE] = "unknown file type!",
	[RESP_UNKNOWN_CONN_TYPE] = "unknown connect type!",
	[RESP_CNT_GET_TASK_CONN] = "cannot get task connect, please try again!",
};
