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
