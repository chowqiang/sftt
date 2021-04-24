#include "mem_pool.h"
#include "session.h"

extern struct mem_pool *g_mp;

struct client_session *client_session_construct(void)
{
	struct client_session *session = mp_malloc(g_mp, sizeof(struct client_session));

	return session;
}

void client_session_deconstruct(struct client_session *ptr)
{
	mp_free(g_mp, ptr);
}

struct server_session *server_session_construct(void)
{
	struct server_session *session = mp_malloc(g_mp, sizeof(struct server_session));

	return session;
}

void server_session_deconstruct(struct server_session *ptr)
{
	mp_free(g_mp, ptr);
}
