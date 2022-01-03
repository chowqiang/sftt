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
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "progress_viewer.h"
#include "utils.h"

int progress2(void)
{                                                                                                                                       
	char split[4] = {'-','/','|','\\'};
	char buf[1024] = {0};
	int i = 0;

	for(; i<100; ++i){
 		buf[i] = '#';
		printf("[%d%%][%c][%s]\r",i,split[i % 4],buf);
		fflush(stdout);
		usleep(200*1000);
	}

	return 0;
}                

int display_progress(int progress, int last_char_count)     
{
    int i = 0;

    for (i = 0; i < last_char_count; i++)
    {
        printf("\b"); 
    }

    for (i = 0; i < progress; i++)
    {
            printf("=");  
    }
    printf(">>");
    for (i += 2; i < 104; i++) 
    {
            printf(" ");
    }
    i = i + printf("[%d%%]", progress);  
    fflush(stdout);

    return i; 
}

struct trans_session
{
	char *prompt;
	long total_size;
	int idx;
	int unit_size;
	long sent_size;
	int udelay;
};

void init_trans_session(struct trans_session *ts, char *prompt, long total_size,
	int unit_size, int udelay)
{
	ts->prompt = prompt;
	ts->total_size = total_size;
	ts->unit_size = unit_size;
	ts->udelay = udelay;
	ts->idx = 0;
	ts->sent_size = 0;
}

int recv_trans_fname(struct trans_session *ts, char fname[], int len)
{
	strncpy(fname, ts->prompt, len - 1);
	fname[len - 1] = 0;
	
	return 0;
}

int recv_trans_data(struct trans_session *ts)
{
	int size;
	usleep(ts->udelay);

	if (ts->idx == 0) {
		ts->idx++;

		return ts->total_size;
	} if (ts->sent_size + ts->unit_size <= ts->total_size) {
		ts->idx++;
		ts->sent_size += ts->unit_size;

		return ts->unit_size;
	} else {
		size = (ts->total_size - ts->sent_size);
		size = size < 0 ? 0 : size;
		if (size)
			ts->idx++;
		ts->sent_size += size;

		return size;
	}
}


int deal_trans_session(struct trans_session *ts)
{
	long total_size = 0;
	long recv_size = 0;
	char fname[128];
	char info[128];
	float progress, speed;
	double start, now;
	int left;
	struct progress_viewer pv;
	
	if (ts->idx != 0) {
		printf("the initial trans session idx should be zero!\n");
		return -1;
	}

	start = get_double_time();

	recv_trans_fname(ts, fname, 128);

	start_progress_viewer(&pv, 1000 * 1000);
	total_size = recv_trans_data(ts);

	if (recv_size >= total_size)
		return 0;	

	while (recv_size < total_size) {
		recv_size += recv_trans_data(ts);

		progress = recv_size * 1.0 / total_size;
		now = get_double_time();
		speed = recv_size * 1.0 / (now - start);
		left = (total_size - recv_size) / speed;
		
		snprintf(info, 128, "%s  %d%% %ld %.1f %d",
			ts->prompt, (int)(progress * 100), recv_size, speed, left);
		show_progress(&pv, info);
		if (recv_size == total_size)
			stop_progress_viewer(&pv, info);
	}


	return 0;
}

void test_progress(void)
{
	struct trans_session ts1, ts2, ts3, ts4;
	
	init_trans_session(&ts1, "file1.txt", 10000000, 1024, 1000);	
	deal_trans_session(&ts1);

	init_trans_session(&ts2, "file2.txt", 10000000, 1024, 1000);	
	deal_trans_session(&ts2);

	init_trans_session(&ts3, "file3.txt", 10000000, 1024, 1000);	
	deal_trans_session(&ts3);

	init_trans_session(&ts4, "file4.txt", 10000000, 1024, 1000);	
	deal_trans_session(&ts4);
}

int main(void)
{
	test_progress();

    return 0;
}
