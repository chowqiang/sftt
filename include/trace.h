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

#ifndef _TRACE_H_
#define _TRACE_H_

#include "user.h"
#include "req_resp.h"

#define TRACE_ID_LEN 16	

struct trace_info {
	char id[TRACE_ID_LEN + 1];
	struct user_base_info *user;
	struct trace_info_ops *ops;
};

struct trace_info_ops {
	int (*set_user)(struct trace_info *trace, struct user_base_info *user);
	struct sds *(*to_str)(struct trace_info *trace);
};

int trace_info_set_user(struct trace_info *trace, struct user_base_info *user);
struct sds *trace_info_to_str(struct trace_info *trace);

#endif
