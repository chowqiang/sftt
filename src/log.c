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

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include "base.h"
#include "config.h"
#include "log.h"
#include "mem_pool.h"
#include "msg_queue.h"
#include "ratelimit.h"
#include "req_resp.h"
#include "utils.h"

extern struct mem_pool *g_mp;

static enum log_type type = UNKNOWN_LOG;
int verbose_level = 0;

static char client_log_dir[DIR_PATH_MAX_LEN];
static char client_log_prefix[32];

static FILE *server_log_fp = NULL;

struct ratelimit_state *client_limit;

struct ratelimit_state *server_limit;

int get_log_msqid(int create_flag);

void set_log_type(enum log_type t)
{
	type = t;
}

static inline char *get_log_level_desc(int level)
{
	switch (level) {
	case LOG_DEBUG:
		return "DEBUG";
	case LOG_INFO:
		return "INFO";
	case LOG_WARN:
		return "WARN";
	case LOG_ERROR:
		return "ERROR";
	default:
		return "*";
	}
}

void get_log_file_name(char *dir, char *prefix, char *file_name, int max_len)
{
	char suffix[16];

	bzero(suffix, 0);

	ymd_hm_str(suffix, 15);

	int plen = strlen(prefix);
	if (plen == 0) {
		snprintf(file_name, max_len - 1, "%s/%s.log", dir, suffix);
	} else {
		snprintf(file_name, max_len - 1, "%s/%s.%s.log", dir, prefix, suffix);
	}
}

int logger_init(char *dir, char *prefix)
{
	strcpy(client_log_dir, dir);
	strcpy(client_log_prefix, prefix);

	client_limit = new(ratelimit_state, 10, 1000);
	assert(client_limit != NULL);

	return 0;
}

void *logger_daemon(void *args)
{
	char *dir, *prefix;
	int msqid;
    	key_t key;
    	struct msgbuf msg;
	char file1[FILE_NAME_MAX_LEN];
	char file2[FILE_NAME_MAX_LEN];
	int ret = 0;
	struct logger_init_ctx *ctx = args;

	server_limit = new(ratelimit_state, 10, 1000);
	assert(server_limit != NULL);

	dir = ctx->dir;
	prefix = ctx->prefix;

	msqid = get_log_msqid(1);
	if (msqid == -1) {
	    perror("get log msqid failed");
		return NULL;
	}

	get_log_file_name(dir, prefix, file1, FILE_NAME_MAX_LEN);
	server_log_fp = fopen(file1, "a");
	if (server_log_fp == NULL) {
	    printf("%d: open log file %s failed!\n", __LINE__, file1);
		return NULL;
	}

	printf("logger thread begin to work ...\n");
	for (;;) {
		memset(&msg, 0, sizeof(msg));
		ret = msgrcv(msqid, &msg, MSG_MAX_LEN, MSG_TYPE_LOG, 0);
		if (ret == -1) {
			perror("msgrcv failed");
			sleep(1);
			continue;
		}
		//printf("received msg: length=%d\n", msg.length);
		if (msg.length <= 0) {
			printf("seems to be a wrong msg, length=%d\n", msg.length);
			continue;
		}
		if (ratelimit_try_inc(server_limit) == false) {
			printf("hit ratelimit ...\n");
			continue;
		}
		get_log_file_name(dir, prefix, file2, FILE_NAME_MAX_LEN);
		if (strcmp(file1, file2)) {
			fclose(server_log_fp);
			server_log_fp = fopen(file2, "a");
			if (server_log_fp == NULL) {
				printf("%d: open log file %s failed!\n", __LINE__, file2);
				continue;
			}
			strcpy(file1, file2);
		}
		fwrite(msg.buf, msg.length, 1, server_log_fp);
		fflush(server_log_fp);
	}

	return NULL;
}

int get_log_msqid(int create_flag)
{
	key_t key;
	int msqid;
	int msgflag;

	if ((key = ftok(SFTT_LOG_MSQKEY_FILE, 'S')) == -1) {
		printf("sfttd ftok failed!\n"
			"\tFile \"" SFTT_LOG_MSQKEY_FILE "\" is existed?\n");
		return -1;
	}

	msgflag = (create_flag ? IPC_CREAT : 0) | 0666;
	if ((msqid = msgget(key, msgflag)) == -1 && (errno != ENOENT)) {
		perror("sfftd msgget failed");
		printf("key: 0x%0x, msgflag: 0x%0x\n", key, msgflag);
		return -1;
	}

	return msqid;
}

void logger_exit(int sig)
{
	add_log(LOG_INFO, "log server is stop ...");

	if (server_log_fp) {
		fclose(server_log_fp);
	}

	int msqid = get_log_msqid(0);
	if (msqid == -1) {
		return ;
	}

	msgctl(msqid, IPC_RMID, NULL);
}

int add_client_log(int level, const char *fmt, va_list args)
{
	char log_file[FILE_NAME_MAX_LEN];
	char buf[MSG_MAX_LEN];
	char now[32];

	if (ratelimit_try_inc(client_limit) == false) {
		return -1;
	}

	char *desc = get_log_level_desc(level);
	now_time_str(now, 31);

	int ret = sprintf(buf, "[%s][%s]", desc, now);

	ret += vsnprintf(buf + ret, MSG_MAX_LEN - ret - 1, fmt, args);

	//printf("client log dir: %s\n", client_log_dir);
	get_log_file_name(client_log_dir, client_log_prefix, log_file, FILE_NAME_MAX_LEN);
	FILE *client_log_fp = fopen(log_file, "a");
	if (client_log_fp == NULL) {
		return -1;
	}

	fprintf(client_log_fp, "%s\n", buf);
	fflush(client_log_fp);

	fclose(client_log_fp);

	return 0;
}

int add_server_log(int level, const char *fmt, va_list args)
{
	struct msgbuf msg;
	int msqid;
	char *buf = msg.buf;
	char now[32];
	char *desc;
	int ret;

	desc = get_log_level_desc(level);

	msqid = get_log_msqid(0);
	if (msqid == -1) {
		if (errno == ENOENT) {
			msqid = get_log_msqid(1);
		}
		if (msqid == -1) {
			return -1;
		}
	}

	msg.mtype = MSG_TYPE_LOG;
	now_time_str(now, 31);

	ret = sprintf(buf, "[%s][%s]", desc, now);

	ret += vsnprintf(buf + ret, MSG_MAX_LEN - ret - 1, fmt, args);

	strcat(buf + ret, "\n");
	ret += 1;

	msg.length = ret;

	if (msgsnd(msqid, &msg, MSG_MAX_LEN, IPC_NOWAIT) < 0) {
		perror("send log msg failed");
		return -1;
	}

	return 0;
}

int add_log(int level, const char *fmt, ...)
{
	int ret = -1;

	va_list args;
	va_start(args, fmt);

	switch (type) {
	case CLIENT_LOG:
		ret = add_client_log(level, fmt, args);
		break;
	case SERVER_LOG:
		ret = add_server_log(level, fmt, args);
		break;
	default:
		break;
	}

	va_end(args);

	return ret;
}

struct logger *logger_construct(char *store_dir, int interval, int burst)
{
	struct logger *log;

	assert(strlen(store_dir) < DIR_PATH_MAX_LEN);

	log = (struct logger *)mp_malloc(g_mp, __func__, sizeof(struct logger));
	assert(log != NULL);

	log->mutex = new(pthread_mutex);
	assert(log->mutex != NULL);

	log->limit = new(ratelimit_state, interval, burst);
	assert(log->limit != NULL);

	strncpy(log->store_dir, store_dir, strlen(store_dir));

	return log;
}

void logger_destruct(struct logger *ptr)
{
	ptr->mutex->ops->destroy(ptr->mutex);
	mp_free(g_mp, ptr);
}

int do_log(struct logger *log, struct trace_info *trace,
	int level, const char *fmt, va_list args)
{
	char log_file[FILE_NAME_MAX_LEN];
	char buf[MSG_MAX_LEN];
	char now[32];

	if (ratelimit_try_inc(client_limit) == false) {
		return -1;
	}

	char *desc = get_log_level_desc(level);
	now_time_str(now, 31);

	int ret = sprintf(buf, "[%s][%s]", desc, now);

	ret += vsnprintf(buf + ret, MSG_MAX_LEN - ret - 1, fmt, args);

	//printf("client log dir: %s\n", client_log_dir);
	get_log_file_name(client_log_dir, client_log_prefix, log_file, FILE_NAME_MAX_LEN);
	FILE *client_log_fp = fopen(log_file, "a");
	if (client_log_fp == NULL) {
		return -1;
	}

	fprintf(client_log_fp, "%s\n", buf);
	fflush(client_log_fp);

	fclose(client_log_fp);

	return 0;
}

int log_info(struct logger* log, struct trace_info *trace, const char *fmt, ...)
{
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = do_log(log, trace, LOG_INFO, fmt, args);
	va_end(args);

	return ret;
}
