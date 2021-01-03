#include <time.h>
#include <stdio.h>
#include "utils.h"

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
