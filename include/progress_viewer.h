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

#ifndef _PROGRESS_VIEWER_H_
#define _PROGRESS_VIEWER_H_

#define SIZE_1G	(1024 * 1024 * 1024)
#define SIZE_1M (1024 * 1024)
#define SIZE_1K 1024

struct progress_viewer
{
	int idx;
	int udelay;
	double last;
	int char_cnt;
};

void format_trans_speed(long speed, char *buf, int max_len);
void format_left_time(int left, char *buf, int max_len);
void format_trans_size(long size, char *buf, int max_len);

void start_progress_viewer(struct progress_viewer *pv, int udelay);
void show_progress(struct progress_viewer *pv, char *info);
void stop_progress_viewer(struct progress_viewer *pv, char *info); 

#endif
