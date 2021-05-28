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
	RESP_OK
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
