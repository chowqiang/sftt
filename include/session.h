#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdbool.h>
#include "config.h"
#include "md5.h"
#include "user.h"
#include "req_rsp.h"

struct client_session {
	char session_id[SESSION_ID_LEN];
};

struct server_session {
	char session_id[SESSION_ID_LEN];
	bool is_validate;
	struct user_info *user;
	char pwd[DIR_PATH_MAX_LEN];

};

#endif
