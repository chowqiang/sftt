#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "utils.h"

int gen_int(int begin, int end)
{
	time_t t;

	assert(end > begin);
	t = time(NULL);
	srand((unsigned)(t));

	return rand() % (end - begin) + begin;
}

char gen_char(char begin, char end)
{
	time_t t;

	assert(end > begin);
	t = time(NULL);
	srand((unsigned)(t));

	return rand() % (end - begin) + begin;
}

int gen_id(void)
{
	int MAX_ID = (1 << 30);
	int MIN_ID = ( 1 << 29);

	return gen_int(MIN_ID, MAX_ID);
}

void gen_session_id(char *buf, int len)
{
	int i;

	for (i = 0; i < len - 1; ++i) {
		if (i % 2) {
			buf[i] = gen_char('a', 'z');
		} else {
			buf[i] = gen_char('0', '9');
		}
	}

	buf[len] = 0;
}

bool is_int(char *buf, int *num)
{
	if (!(buf[0] == '-' || buf[0] == '+' || isdigit(buf[0]))) {
		return false;
	}

	if (strlen(buf) > 11) {
		return false;
	}

	int i;
	for (i = 1; buf[i]; ++i) {
		if (!isdigit(buf[i])) {
			return false;
		}
	}

	return atol(buf) == atoi(buf);
}

int ymd_hm_str(char *buf, int max_len) {
	time_t ts = time(NULL);
	const char *format = "%Y%m%d%H";
    struct tm lt;
	int ret = 0;

    (void) localtime_r(&ts, &lt);
    if ((ret = strftime(buf, max_len, format, &lt)) == 0) {
		return sprintf(buf, "unknown");
	}

	return ret;
}

int now_time_str(char *buf, int max_len) {
	time_t ts = time(NULL);
	return ts_to_str((uint64_t)ts, buf, max_len);
}

int ts_to_str(uint64_t ts, char *buf, int max_len) {
	time_t t = (time_t)ts;
    const char *format = "%Y-%m-%d %H:%M:%S";
    struct tm lt;
	int ret = 0;

    (void) localtime_r(&t, &lt);
    if ((ret = strftime(buf, max_len, format, &lt)) == 0) {
		return sprintf(buf, "unknown");
	}

	return ret;
}

time_t get_ts(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);

	return (time_t)t.tv_sec;
}

void strip(char *line)
{
	int len = strlen(line);

	while (len > 0 && line[len - 1] == ' ') {
		line[--len] = 0;
	}

	if (len < 1) {
		return ;
	}

	int i = 0;
	while (i < len && line[i] == ' ') {
		++i;
	}
	if (i >= len) {
		line[0] = 0;
		return ;
	}

	int j = 0;
	for (j = 0; i <= len; ++i, ++j) {
		line[j] = line[i];
	}
}

void simplify_path(char *path)
{
	int i = 0, j = 0, len = 0;
	char *tmp = NULL;
	bool slash = false;

	len = strlen(path);
	if (len == 0)
		return ;

	tmp = (char *)malloc(sizeof(char) * (len + 1));
	assert(tmp);

	i = len - 1;
	while (i >= 0 && path[i] == '/')
		path[i--] = 0;

	i = 0;
	while (path[i]) {
		if (path[i] == '/' && slash == true) {
			++i;
			continue;
		}
		tmp[j++] = path[i];
		slash = path[i] == '/';
		++i;
	}

	tmp[j] = 0;
	strcpy(path, tmp);

	free(tmp);
}

int get_right_most_path(char *path, char *sub_path)
{
	int len = 0, idx = 0;
	char *pos = NULL;

	if (path == NULL || sub_path == NULL)
		return -1;

	simplify_path(path);

	if ((len = strlen(path)) == 0) {
		sub_path[0] = 0;
		return 0;
	}

	if ((pos = rindex(path, '/')) == NULL) {
		strcpy(sub_path, path);
		return 0;
	}

	strcpy(sub_path, pos + 1);

	return 0;
}
