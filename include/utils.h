#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

void generate_session_id(char *buf, int len);

int now_time_str(char *buf, int max_len);

int ts_to_str(uint64_t ts, char *buf, int max_len);

int ymd_hm_str(char *buf, int max_len);

time_t get_ts(void);

#endif
