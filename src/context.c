#include <stdlib.h>
#include "context.h"

struct context *g_ctx = NULL;

struct context *get_current_context(void)
{
	if (g_ctx == NULL) {
		g_ctx = malloc(sizeof(struct context)); 
	}

	return g_ctx;
}
