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

#ifndef _LOG_H_
#define _LOG_H_

#include "config.h"
#include "req_resp.h"
#include "trace.h"

#define LOG_DEBUG	1
#define LOG_INFO	2
#define LOG_WARN	3
#define LOG_ERROR	4

#define SFTT_LOG_MSQKEY_FILE	"/var/lib/sfttd/msgkey"

#define LOGGER_PREFIX_LEN	32

enum log_type {
	CLIENT_LOG,
	SERVER_LOG,
	UNKNOWN_LOG,
};

struct logger {
	struct pthread_mutex *mutex;
	struct ratelimit_state *limit;
	char store_dir[DIR_PATH_MAX_LEN];
};

struct logger_init_ctx {
	char dir[DIR_PATH_MAX_LEN];
	char prefix[LOGGER_PREFIX_LEN];
};

int logger_init(char *dir, char *prefix);

void *logger_daemon(void *args);

void logger_exit(int sig);

int add_log(int level, const char *fmt, ...);

void set_log_type(enum log_type t);

int log_info(struct logger* log, struct trace_info *trace, const char *fmt, ...);

struct logger *logger_construct(char *store_dir, int interval, int burst);

void logger_destruct(struct logger *ptr);

#endif
