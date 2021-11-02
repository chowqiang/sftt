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

#include <stdlib.h>
#include <string.h>
#include "context.h"

struct context *g_ctx = NULL;

void set_current_context(char *name)
{
	if (g_ctx == NULL)
		g_ctx = get_current_context();

	if (name == NULL || strlen(name) >= 16)
		return;

	strcpy(g_ctx->name, name);
}

struct context *get_current_context(void)
{
	if (g_ctx == NULL) {
		g_ctx = malloc(sizeof(struct context));
	}

	return g_ctx;
}
