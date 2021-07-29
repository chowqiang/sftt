#include <stdlib.h>
#include <string.h>
#include "context.h"

struct context *g_ctx = NULL;

void set_current_context(char *shm_key)
{
	if (g_ctx == NULL)
		g_ctx == get_current_context();

	if (shm_key == NULL || strlen(shm_key) >= SHM_KEY_NAME_LEN)
		return;

	strcpy(g_ctx->shm_key, shm_key);
}

struct context *get_current_context(void)
{
	if (g_ctx == NULL) {
		g_ctx = malloc(sizeof(struct context)); 
	}

	return g_ctx;
}
