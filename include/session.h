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
struct client_info {
	int status;
	char pwd[DIR_PATH_MAX_LEN];
};

struct thread_info {
	pthread_t tid;
};

enum process_status {
	ACTIVE,
	EXITED
};

enum client_status {
	VALIDATED,
	INVALIDATED
};

struct client_session {
	int connect_fd;
	struct client_info cinfo;
	struct thread_info tinfo;
	enum process_status status;
	char session_id[SESSION_ID_LEN];
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
