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

#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdbool.h>
#include "config.h"
#include "md5.h"
#include "user.h"
#include "req_rsp.h"

#define MAX_CLIENT_NUM 32
/*
 *
 */

enum process_status {
	RUNNING,
	EXITED
};

struct thread_info {
	pthread_t tid;
	enum process_status status;
};

enum client_status {
	ACTIVE,
	INACTIVE
};

enum session_status {
	VALIDATED,
	INVALIDATED
};

enum resp_status {
	SESSION_INVALID,
	SERVER_EXCEPTION,
	RESP_OK,
	CANNOT_CD,
	CONTINUE,
};

struct client_session {
	int connect_fd;
	struct thread_info tinfo;
	enum client_status status;
	char pwd[DIR_PATH_MAX_LEN];
	char session_id[SESSION_ID_LEN];
	struct user_base_info user;
};

struct server_session {
	char session_id[SESSION_ID_LEN];
	bool is_validate;
	struct user_base_info *user;
	char pwd[DIR_PATH_MAX_LEN];

};

struct client_session *client_session_construct(void);
void client_session_deconstruct(struct client_session *ptr);

struct server_session *server_session_construct(void);
void server_session_deconstruct(struct server_session *ptr);

#endif
