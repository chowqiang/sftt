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

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg_queue.h"

int send_msg(struct msg_queue *queue, struct msgbuf *msg)
{
	int ret;

	ret = msgsnd(queue->msqid, msg, MSG_MAX_LEN, IPC_NOWAIT);
	if (errno) {
		perror("msgsnd failed");
	}

	return ret;
}

int recv_msg(struct msg_queue *queue, struct msgbuf *msg)
{

	int ret;

	ret = msgrcv(queue->msqid, msg, MSG_MAX_LEN, msg->mtype, 0);
	if (errno) {
		perror("msgrecv failed");
	}

	return ret;
}

struct msg_queue *create_msg_queue(char *name)
{
	key_t key;
	int msqid;
	int msgflag;
	struct msg_queue *queue;

	if ((key = ftok(name, 'S')) == -1) {
		perror("ftok failed");
		printf("msg queue ftok failed!\n"
			"\tFile \"%s\" is existed?\n", name);
		return NULL;
	}

	msgflag = IPC_CREAT | 0666;
	if ((msqid = msgget(key, msgflag)) == -1) {
		perror("msgget failed");
		printf("key: 0x%0x, msgflag: 0x%0x\n", key, msgflag);
		return NULL;
	}

	queue = (struct msg_queue *)malloc(sizeof(struct msg_queue));
	assert(queue != NULL);

	queue->msqid = msqid;

	return queue;
}

struct msg_queue *get_msg_queue(char *name)
{
	key_t key;
	int msqid;
	int msgflag;
	struct msg_queue *queue;

	if ((key = ftok(name, 'S')) == -1) {
		perror("ftok failed");
		printf("msg queue ftok failed!\n"
			"\tFile \"%s\" is existed?\n", name);
		return NULL;
	}

	msgflag = 0666;
	if ((msqid = msgget(key, msgflag)) == -1) {
		perror("msgget failed");
		printf("key: 0x%0x, msgflag: 0x%0x\n", key, msgflag);
		return NULL;
	}

	queue = (struct msg_queue *)malloc(sizeof(struct msg_queue));
	assert(queue != NULL);

	queue->msqid = msqid;

	return queue;
}

void delete_msg_queue(char *name)
{
	struct msg_queue *queue;

	queue = get_msg_queue(name);
	if (queue == NULL) {

		return;
	}

	msgctl(queue->msqid, IPC_RMID, NULL);
}

