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
#include <unistd.h>
#include "context.h"
#include "mem_pool.h"
#include "msg_queue.h"

int main(void)
{
	struct msgbuf msg;
	struct msg_queue *queue;
	int ret;
	struct mem_pool_stat *stat;

	set_current_context("mpstat");

	delete_msg_queue(MEM_POOL_STAT_MSGKEY);

	queue = create_msg_queue(MEM_POOL_STAT_MSGKEY);
	printf("queue: 0x%0lx\n", queue);

	if (queue == NULL)
		get_msg_queue(MEM_POOL_STAT_MSGKEY);

	printf("queue: 0x%0lx\n", queue);

	printf("press any key to continue ...\n");
	getchar();

	if (queue == NULL) {
		printf("cannot get msg queue\n");
		return -1;
	}

	printf("%s:%d, msqid=%d\n", __func__, __LINE__, queue->msqid);

	msg.mtype = MSG_TYPE_MPSTAT;
	for (;;) {
		//system("clear");
		ret = recv_msg(queue, &msg);
#if 0
		if (ret < 0)
			continue;
#endif

		stat = (struct mem_pool_stat *)msg.mtext;

		printf("endpoint\tpid\t"
			"total_size\ttotal_nodes\t"
			"using_nodes\tfree_nodes\n");

		printf("%s\t%d\t"
			"%d\t%d\t"
			"%d\t%d\n", msg.name, msg.pid,
			stat->total_size, stat->total_nodes,
			stat->using_nodes, stat->free_nodes);

		sleep(1);
	}


	return 0;
}
