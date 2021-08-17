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

#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_

#define MSG_MAX_LEN	256
#define MSQ_NAME_LEN	128

#define MSG_TYPE_LOG		1
#define MSG_TYPE_MPSTAT		2

#define MSG_QUEUE_FIFO_FILE	"/tmp/sftt.msq.fifo"

enum msq_type {
	MSQ_TYPE_IPC,
	MSQ_TYPE_FILE,
	MSQ_TYPE_NET,
	MSQ_TYPE_CNT
};

struct msgbuf {
	long mtype;
	union {
		char mtext[MSG_MAX_LEN];
		struct {
			char name[16];
			int pid;
			int length;
			char buf[0];
		};
	};
};

struct msg_queue {
	char name[MSQ_NAME_LEN];
	enum msq_type type;
	int msqid;
	int fd;
	int sock;
	struct msq_ops *ops;
	struct list_head list;
};

/*
 * Message queue operations.
 */
struct msq_ops {
	int (*send_msg)(struct msg_queue *queue, struct msgbuf *msg);
	int (*recv_msg)(struct msg_queue *queue, struct msgbuf *msg);
};

int send_msg(struct msg_queue *queue, struct msgbuf *msg);

int recv_msg(struct msg_queue *queue, struct msgbuf *msg);

struct msg_queue *create_msg_queue(char *name, enum msq_type type);

struct msg_queue *get_msg_queue(char *name, enum msq_type type);

void delete_msg_queue(struct msg_queue *queue);

#endif
