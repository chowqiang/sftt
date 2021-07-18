#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "shm_space.h" 

struct context {
	char shm_key[SHM_KEY_NAME_LEN];
};

struct context *get_current_context(void);

#endif
