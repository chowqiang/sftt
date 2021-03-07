#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdbool.h>
#include "config.h"
#include "md5.h"
#include "user.h"
#include "xdr.h"

typedef struct client_session {
	char session_id[SESSION_ID_LEN];
} client_session;

typedef struct server_session {
	char session_id[SESSION_ID_LEN];
	bool is_validate;
	user_info *user;
	char pwd[DIR_PATH_MAX_LEN];

} server_session;

#endif
