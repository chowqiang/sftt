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

#ifndef _COMMON_H_
#define _COMMON_H_

#include "req_resp.h"

#define USER_NAME_MAX_LEN	XDR_USER_NAME_MAX_LEN
#define PASSWD_MD5_LEN		XDR_PASSWD_MD5_LEN
#define SESSION_ID_LEN		XDR_SESSION_ID_LEN
#define DIR_NAME_MAX_LEN	XDR_DIR_NAME_MAX_LEN
#define FILE_NAME_MAX_LEN	XDR_FILE_NAME_MAX_LEN
#define DIR_PATH_MAX_LEN 	XDR_DIR_PATH_MAX_LEN
#define FILE_PATH_MAX_LEN	XDR_FILE_PATH_MAX_LEN
#define FILE_ENTRY_MAX_CNT	XDR_FILE_ENTRY_MAX_CNT
#define CONTENT_BLOCK_SIZE	XDR_CONTENT_BLOCK_SIZE
#define NET_MSG_MAX_LEN		XDR_NET_MSG_MAX_LEN
#define CMD_MAX_LEN		XDR_CMD_MAX_LEN
#define CMD_RET_BATCH_LEN	XDR_CMD_RET_BATCH_LEN
#define IPV4_MAX_LEN		XDR_IPV4_MAX_LEN
#define LOGGED_IN_USER_MAX_CNT	XDR_LOGGED_IN_USER_MAX_CNT
#define WRITE_MSG_MAX_LEN	XDR_WRITE_MSG_MAX_LEN
#define RESP_MESSAGE_MAX_LEN	XDR_RESP_MESSAGE_MAX_LEN
#define DIRECT_CMD_RESP_MAX_LEN	XDR_DIRECT_CMD_RESP_MAX_LEN
#define CONNECT_ID_LEN		XDR_CONNECT_ID_LEN
#define REQ_RESP_FLAG_NONE	XDR_REQ_RESP_FLAG_NONE
#define REQ_RESP_FLAG_NEXT	XDR_REQ_RESP_FLAG_NEXT
#define REQ_RESP_FLAG_STOP	XDR_REQ_RESP_FLAG_STOP

enum free_mode {
	FREE_MODE_NOTHING,
	FREE_MODE_MP_FREE,
	FREE_MODE_FREE
};

#endif
