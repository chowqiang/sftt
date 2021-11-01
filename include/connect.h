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

#ifndef _CONNECT_H_
#define _CONNECT_H_

#include <pthread.h>
#include "list.h"
#include "req_resp.h"
#include "thread.h"

#define CLIENT_MAX_TASK_CONN 5

enum connect_result {
	CONN_RET_CONNECT_FAILED,
	CONN_RET_VALIDATE_FAILED,
	CONN_RET_SERVER_BUSYING,
	CONN_RET_CONNECT_SUCCESS,
};

enum connect_type {
	CONN_TYPE_CTRL,
	CONN_TYPE_DATA,
	CONN_TYPE_TASK,
};

struct client_sock_conn {
	bool is_using;
	int sock;
	int port;
	enum connect_type type;
	char connect_id[CONNECT_ID_LEN];
	struct thread_info tinfo;
	struct list_head list;
};

struct server_context {
	int sock;
};

int make_connect(char *ip, int port);

pthread_t start_server(int port, void *(*func)(void *arg));

#endif
