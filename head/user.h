#ifndef _USER_H_
#define _USER_H_

typedef struct user_info {
	char name[MAX_NAME_LEN];
	
} user_info;

bool user_add(user_info ui);

#endif
