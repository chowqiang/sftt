#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include "config.h"
#include "log.h"
#include "utils.h"

typedef struct
{
    long    mtype;
    char    mtext[LOG_STR_MAX_LEN];
} msgbuf;

static log_type type = UNKNOWN_LOG;

static char client_log_dir[DIR_PATH_MAX_LEN];
static char client_log_prefix[32];

static FILE *server_log_fp = NULL;

int get_log_msqid(int create_flag);

void set_log_type(log_type t)
{
	type = t;
}

static inline char *get_log_level_desc(int level) {
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

void get_log_file_name(char *dir, char *prefix, char *file_name, int max_len) {
	char suffix[16];
	ymd_hm_str(suffix, 15);

	int plen = strlen(prefix);
	if (plen == 0) {
		snprintf(file_name, max_len - 1, "%s/%s.log", dir, suffix);
	} else {
		snprintf(file_name, max_len - 1, "%s/%s.%s.log", dir, prefix, suffix);
	}
}

void logger_init(char *dir, char *prefix)
{
	strcpy(client_log_dir, dir);
	strcpy(client_log_prefix, prefix);
}

void logger_daemon(char *dir, char *prefix) {
	int msqid;
    key_t key;
    msgbuf msg;

	msqid = get_log_msqid(1);
	if (msqid == -1) {
	    perror("get log msqid failed");
		return ;
	}

	char file1[FILE_NAME_MAX_LEN];
	char file2[FILE_NAME_MAX_LEN];

	get_log_file_name(dir, prefix, file1, FILE_NAME_MAX_LEN);
	server_log_fp = fopen(file1, "a");
	if (server_log_fp == NULL) {
	    perror("open log file failed");
		return ;
	}

	int ret = 0;
	for (;;) {
		ret = msgrcv(msqid, &msg, LOG_STR_MAX_LEN, LOG_MSG_TYPE, 0);
		get_log_file_name(dir, prefix, file2, FILE_NAME_MAX_LEN);
		if (strcmp(file1, file2)) {
			fclose(server_log_fp);
			server_log_fp = fopen(file2, "a");
			if (server_log_fp == NULL) {
			    perror("open log file failed");
				continue;
			}
			strcpy(file1, file2);
		}
		fwrite(msg.mtext, ret, 1, server_log_fp);
		fflush(server_log_fp);
	}
}

int get_log_msqid(int create_flag) {
	key_t key;
	int msqid;

	if ((key = ftok(SFTT_LOG_MSQKEY_FILE, 'S')) == -1) {
		printf("sfttd ftok failed!\n"
			"\tFile \"" SFTT_LOG_MSQKEY_FILE "\" is existed?\n");
		return -1;
	}

	int msgflag = (create_flag ? IPC_CREAT : 0) | 0666;
	if ((msqid = msgget(key, msgflag)) == -1 && (errno != ENOENT)) {
		perror("sfftd msgget failed");
		printf("key: 0x%0x, msgflag: 0x%0x\n", key, msgflag);
		return -1;
	}

	return msqid;
}

void logger_exit(int sig) {
	add_log(LOG_INFO, "log server is stop ...");

	if (server_log_fp) {
		fclose(server_log_fp);
	}

	int msqid = get_log_msqid(0);
	if (msqid == -1) {
		return ;
	}

	printf("log server is stop ...\n");
	msgctl(msqid, IPC_RMID, NULL);

	exit(0);
}

int add_client_log(int level, const char *fmt, va_list args)
{
	char log_file[FILE_NAME_MAX_LEN];
	char buf[LOG_STR_MAX_LEN];
	char now[32];

	char *desc = get_log_level_desc(level);
	now_time_str(now, 31);

	int ret = sprintf(buf, "[%s] %s ", desc, now);

	//va_list args;
	//va_start(args, fmt);
	ret += vsnprintf(buf + ret, LOG_STR_MAX_LEN - ret - 1, fmt, args);
	//va_end(args);

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
	int msqid = get_log_msqid(0);
	if (msqid == -1) {
		if (errno == ENOENT) {
			msqid = get_log_msqid(1);
		}
		if (msqid == -1) {
			return -1;
		}
	}

	msgbuf msg;
	msg.mtype = LOG_MSG_TYPE;
	char *buf = msg.mtext;
	char now[32];

	char *desc = get_log_level_desc(level);
	now_time_str(now, 31);

	int ret = sprintf(buf, "[%s] %s ", desc, now);

	//va_list args;
	//va_start(args, fmt);
	ret += vsnprintf(buf + ret, LOG_STR_MAX_LEN - ret - 1, fmt, args);
	//va_end(args);

	strcat(buf + ret, "\n");
	ret += 1;

	if (msgsnd(msqid, &msg, ret, IPC_NOWAIT) < 0) {
		printf("send log msg failed!\n");
		return -1;
	}

	return 0;
}

int add_log(int level, const char *fmt, ...) {
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
