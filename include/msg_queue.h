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

#define MSG_MAX_LEN	4096

#define MSG_TYPE_LOG		1
#define MSG_TYPE_MPSTAT		2

struct msgbuf {
	long mtype;
	char name[16];
	int pid;
	int length;
	char mtext[MSG_MAX_LEN];
};

struct msg_queue {
	int msqid;
};

int send_msg(struct msg_queue *queue, struct msgbuf *msg);

int recv_msg(struct msg_queue *queue, struct msgbuf *msg);

struct msg_queue *create_msg_queue(char *name);

struct msg_queue *get_msg_queue(char *name);

void delete_msg_queue(char *name);

#endif
