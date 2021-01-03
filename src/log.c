#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdarg.h>
#include "config.h"
#include "log.h"

typedef struct
{
    long    mtype;
    char    mtext[LOG_STR_MAX_LEN];
} msgbuf;

FILE *log_fp = NULL;

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

void logger_daemon(char *dir, char *prefix) {
	int msqid;
    key_t key;
    msgbuf msg;

	msqid = get_log_msqid(1);
	if (msqid == -1) {
		return ;
	}

	char file1[FILE_NAME_MAX_LEN];
	char file2[FILE_NAME_MAX_LEN];

	get_log_file_name(dir, prefix, file1, FILE_NAME_MAX_LEN);
	log_fp = fopen(file1, "a");
	if (log_fp == NULL) {
		return ;
	}

	int ret = 0;
	for (;;) {
		ret = msgrcv(msqid, &msg, LOG_STR_MAX_LEN, LOG_MSG_TYPE, 0);
		get_log_file_name(dir, prefix, file2, FILE_NAME_MAX_LEN);
		if (strcmp(file1, file2)) {
			fclose(log_fp);
			log_fp = fopen(file2, "a");
			if (log_fp == NULL) {
				continue;
			}
			strcpy(file1, file2);
		}
		fwrite(msg.mtext, ret, 1, log_fp);
	}
}

int get_log_msqid(int create_flag) {
	key_t key;
	int msqid;

	if ((key = ftok(SFTT_LOG_MSQKEY_FILE, 'S')) == -1) {
		printf(PROC_NAME " ftok failed!\n"
			"\tFile \"" SFTT_LOG_MSQKEY_FILE "\" is existed?\n");
		return -1;
	}

	int msgflag = (create_flag ? IPC_CREAT : 0) | 0666;
	if ((msqid = msgget(key, msgflag)) == -1) {
		printf(PROC_NAME " msgget failed! msgflag: 0x%0x\n", msgflag);
		return -1;
	}

	return msqid;
}

void logger_exit(int sig) {
	if (log_fp) {
		fclose(log_fp);
	}

	int msqid = get_log_msqid(0);
	if (msqid == -1) {
		return ;
	}

	msgctl(msqid, IPC_RMID, NULL);
}

int add_log(int level, const char *fmt, ...) {
	int msqid = get_log_msqid(0);
	if (msqid == -1) {
		return -1;
	}

	msgbuf msg;
	msg.mtype = LOG_MSG_TYPE;
	char *buf = msg.mtext;
	char now[32];

	char *desc = get_log_level_desc(level);
	now_time_str(now, 31);

	int ret = sprintf(buf, "[%s] %s ", desc, now);

	va_list args;
	va_start(args, fmt);
	ret += vsnprintf(buf + ret, LOG_STR_MAX_LEN - ret - 1, fmt, args);
	va_end(args);

	strcat(buf + ret, "\n");
	ret += 2;

	if (msgsnd(msqid, &msg, ret, IPC_NOWAIT) < 0) {
		return -1;
	}

	return 0;
}
