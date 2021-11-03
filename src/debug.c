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

#include "debug.h"

int default_debug_level = DEBUG_WARN;

void set_client_debug_level(int verbose)
{
	if (verbose == 1) {
		default_debug_level = DEBUG_INFO;
	} else if (verbose == 2) {
		default_debug_level = DEBUG_DEBUG;
	} else if (verbose > 2) {
		default_debug_level = DEBUG_DEBUG;
	} else {
		default_debug_level = DEBUG_WARN;
	}
}

void set_server_debug_level(int verbose)
{
	if (verbose == 1) {
		default_debug_level = DEBUG_DEBUG;
	} else if (verbose > 1) {
		default_debug_level = DEBUG_DEBUG;
	} else {
		default_debug_level = DEBUG_INFO;
	}
}
