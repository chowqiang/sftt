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
#include "atomic.h"
#include "config.h"
#include "connect.h"
#include "list.h"
#include "md5.h"
#include "user.h"
#include "req_resp.h"

#define MAX_CLIENT_NUM 32

enum client_status {
	ACTIVE = 1,
	INACTIVE,
	DISCONNECTED,
};

struct client_session {
	char ip[IPV4_MAX_LEN];
	struct client_sock_conn main_conn;
	struct list_head task_conns;
	struct pthread_mutex *tcs_lock;
	enum client_status status;
	char pwd[DIR_PATH_MAX_LEN];
	char session_id[SESSION_ID_LEN];
	struct user_base_info user;
};

struct client_session *client_session_construct(void);
void client_session_deconstruct(struct client_session *ptr);

#define set_client_active(session) ((session)->status = ACTIVE)
#define set_client_inactive(session) ((session)->status = INACTIVE)
#define set_client_disconnected(session) ((session)->status = DISCONNECTED)

#define client_is_active(session) ((session)->status == ACTIVE)
#define client_is_inactive(session) ((session)->status == ACTIVE)

#define client_connected(session) \
	(client_is_active(session) || client_is_inactive(session))

#define client_task_conns_num(session) \
	({						\
		int __num = 0;				\
		struct client_sock_conn *__p;		\
							\
		list_for_each_entry(__p, &(session)->task_conns, list)	\
			++__num;			\
		__num;					\
	})

#define get_peer_task_conn_by_session(session) \
	({									\
		struct client_sock_conn *__conn, *__ret = NULL;			\
										\
		(session)->tcs_lock->ops->lock((session)->tcs_lock);		\
		list_for_each_entry(__conn, &(session)->task_conns, list) {	\
			if (!atomic16_read(&__conn->is_using)) {			\
				atomic16_set(&__conn->is_using, 1);		\
				__ret = __conn;					\
				break;						\
			}							\
		}								\
		(session)->tcs_lock->ops->unlock((session)->tcs_lock);		\
		__ret;								\
	})

#define put_peer_task_conn(conn)	put_sock_conn(conn)
#endif
