#ifndef _LOG_H_
#define _LOG_H_

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

void logger_init(char *dir, char *prefix);

void logger_daemon(char *dir, char *prefix);

void logger_exit(int sig);

int add_log(int level, const char *fmt, ...);

void set_log_type(enum log_type t);

#endif
