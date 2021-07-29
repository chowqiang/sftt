#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "shm_space.h" 

struct context {
	char shm_key[SHM_KEY_NAME_LEN];
};

void set_current_context(char *shm_key);
struct context *get_current_context(void);

#endif
