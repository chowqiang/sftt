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
#include "progress_viewer.h"
#include "utils.h"

void start_progress_viewer(struct progress_viewer *pv, int udelay)
{
	pv->idx = 0;
	pv->udelay = udelay;
	pv->char_cnt = 0;
}

#if 0
void stop_progress_viewer(struct progress_viewer *pv, char *prompt, float progress,
	long recv_size, float speed, int left)
{
	int i;

	for (i = 0; i < pv->char_cnt; ++i)
		putchar(' ');
	putchar('\r');

	pv->char_cnt = printf("%s  %d%% %ld %.1f %d\n",
			prompt, (int)(progress * 100), recv_size, speed, left);
}

void show_progress(struct progress_viewer *pv, char *prompt, float progress,
	long recv_size, float speed, int left)
{
	double now;
	int i;

	now = get_double_time();
	if (pv->idx == 0 || ((now - pv->last) * 1000000 > pv->udelay)) {
		for (i = 0; i < pv->char_cnt; ++i)
			putchar(' ');
		putchar('\r');

		pv->char_cnt = printf("%s  %d%% %ld %.1f %d\r",
			prompt, (int)(progress * 100), recv_size, speed, left);
		pv->idx++;
		pv->last = now;
    	fflush(stdout);

		return ;
	}
}
#endif
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
