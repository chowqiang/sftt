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

#include <stdio.h>
#include "debug.h"
#include "progress_viewer.h"
#include "utils.h"

void format_trans_speed(long speed, char *buf, int max_len)
{
	DBUG_ENTER(__func__);

	if (speed > SIZE_1G) {
		snprintf(buf, max_len, "%.2fGB/s", speed * 1.0 / SIZE_1G);
	} else if (speed > SIZE_1M) {
		snprintf(buf, max_len, "%.2fMB/s", speed * 1.0 / SIZE_1M);
	} else if (speed > SIZE_1K) {
		snprintf(buf, max_len, "%.2fKB/s", speed * 1.0 / SIZE_1K);
	} else {
		snprintf(buf, max_len, "%dB/s", (int)speed);
	}

	DBUG_VOID_RETURN;
}

void format_left_time(int left, char *buf, int max_len)
{
	DBUG_ENTER(__func__);

	int hour, minute, second;

	if (left > 3600) {
		hour = left / 3600;
		minute = left % 3600 / 60;
		second = left % 60;
		if (hour >= 10)
			snprintf(buf, max_len, "%d:%02d:%02d", hour, minute,
					second);
		else
			snprintf(buf, max_len, "%02d:%02d:%02d", hour, minute,
					second);
	} else {
		minute = left / 60;
		second = left % 60;
		snprintf(buf, max_len, "%02d:%02d", minute, second);
	}

	DBUG_VOID_RETURN;
}

void format_trans_size(long size, char *buf, int max_len)
{
	DBUG_ENTER(__func__);

	if (size > SIZE_1G) {
		snprintf(buf, max_len, "%.2fGB", size * 1.0 / SIZE_1G);
	} else if (size > SIZE_1M) {
		snprintf(buf, max_len, "%.2fMB", size * 1.0 / SIZE_1M);
	} else if (size > SIZE_1K) {
		snprintf(buf, max_len, "%.2fKB", size * 1.0 / SIZE_1K);
	} else {
		snprintf(buf, max_len, "%dB", (int)size);
	}

	DBUG_VOID_RETURN;
}

void start_progress_viewer(struct progress_viewer *pv, int udelay)
{
	pv->idx = 0;
	pv->udelay = udelay;
	pv->char_cnt = 0;
}

void stop_progress_viewer(struct progress_viewer *pv, char *info)
{
	int i;

	for (i = 0; i < pv->char_cnt; ++i)
		putchar(' ');
	putchar('\r');

	pv->char_cnt = printf("%s\n", info);
}

void show_progress(struct progress_viewer *pv, char *info)
{
	double now;
	int i;

	now = get_double_time();
	if (pv->idx == 0 || ((now - pv->last) * 1000000 > pv->udelay)) {
		for (i = 0; i < pv->char_cnt; ++i)
			putchar(' ');
		putchar('\r');

		pv->char_cnt = printf("%s\r", info);
		pv->idx++;
		pv->last = now;
    		fflush(stdout);

		return ;
	}
}
