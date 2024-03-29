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
#include "atomic.h"
#include "common.h"
#include "list.h"

#define CLIENT_MAX_TASK_CONN 	5

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
	volatile int sock;
	volatile int port;
	volatile int last_port;
	enum connect_type type;
	char connect_id[CONNECT_ID_LEN];
	struct list_head list;
	atomic16_t is_using;
	atomic16_t is_updating;
};

#define sock_conn_is_using(conn) ({atomic16_read(&((conn)->is_using)) == 1;})

struct server_context {
	int sock;
};

int make_connect(char *ip, int port);

pthread_t start_server(int port, void *(*func)(void *arg));

int make_socket_non_blocking(int sfd);

int make_socket_blocking(int sfd);

#define put_sock_conn(conn) do {atomic16_set(&((conn)->is_using), 0);} while(0)

#define get_sock_conn(conn) do {atomic16_set(&((conn)->is_using), 1);} while(0)

#define is_conn_using(conn) ({atomic16_read(&((conn)->is_using)) == 1;})

#define set_conn_updating(conn) do {atomic16_set(&((conn)->is_updating), 1);} while(0)

#define clear_conn_updating(conn) do {atomic16_set(&((conn)->is_updating), 0);} while(0)

#define is_conn_updating(conn) ({atomic16_read(&((conn)->is_updating)) == 1;})
#endif
