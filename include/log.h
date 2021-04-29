#ifndef _LOG_H_
#define _LOG_H_

#include "config.h"
#include "trace.h"

#define LOG_STR_MAX_LEN	4096

#define LOG_DEBUG	1
#define LOG_INFO	2
#define LOG_WARN	3
#define LOG_ERROR	4

#define LOG_MSG_TYPE	1

#define SFTT_LOG_MSQKEY_FILE	"/var/lib/sfttd/msgkey"

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

void logger_init(char *dir, char *prefix);

void logger_daemon(char *dir, char *prefix);

void logger_exit(int sig);

int add_log(int level, const char *fmt, ...);

void set_log_type(enum log_type t);

int log_info(struct logger* log, struct trace_info *trace, const char *fmt, ...);

struct logger *logger_construct(char *store_dir, int interval, int burst);

void logger_destruct(struct logger *ptr);

#endif
