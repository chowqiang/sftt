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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "context.h"
#include "mem_pool.h"
#include "msg_queue.h"

int main(void)
{
	struct msgbuf msg;
	struct msg_queue *queue;
	struct mem_pool_stat *stat;
	int ret;

	set_current_context("mpstat");

	queue = create_msg_queue(MEM_POOL_STAT, MSQ_TYPE_FILE);
	printf("queue: 0x%0lx\n", (unsigned long)queue);

	printf("press any key to continue ...\n");
	getchar();

	if (queue == NULL) {
		printf("cannot get msg queue\n");
		return -1;
	}

	msg.mtype = MSG_TYPE_MPSTAT;
	for (;;) {
		//system("clear");
		ret = recv_msg(queue, &msg);
		if (ret == -1) {
			printf("recv msg failed!\n");
			sleep(1);
			continue;
		}

		//printf("msg.name=%s\n", msg.name);
		if (strcmp(msg.name, "server") && strcmp(msg.name, "client"))
			continue;

		stat = (struct mem_pool_stat *)msg.buf;

		printf("endpoint       pid"
			"    total_size    total_nodes"
			"    using_nodes    free_nodes\n");

		printf("%8s%10d"
			"%14ld%15d"
			"%15d%14d\n", msg.name, msg.pid,
			stat->total_size, stat->total_nodes,
			stat->using_nodes, stat->free_nodes);

		sleep(1);
	}


	return 0;
}
