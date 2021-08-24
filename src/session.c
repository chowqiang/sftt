/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mem_pool.h"
#include "session.h"

extern struct mem_pool *g_mp;

struct client_session *client_session_construct(void)
{
	struct client_session *session = mp_malloc(g_mp, __func__, sizeof(struct client_session));

	return session;
}

void client_session_deconstruct(struct client_session *ptr)
{
	mp_free(g_mp, ptr);
}

struct server_session *server_session_construct(void)
{
	struct server_session *session = mp_malloc(g_mp, __func__, sizeof(struct server_session));

	return session;
}

void server_session_deconstruct(struct server_session *ptr)
{
	mp_free(g_mp, ptr);
}
