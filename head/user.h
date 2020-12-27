#ifndef _USER_H_
#define _USER_H_

#include <stdbool.h>
#include "config.h"

typedef struct user_info {
	char name[USER_NAME_MAX_LEN];
	
} user_info;

bool user_add(user_info ui);

#endif
