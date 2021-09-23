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
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "list.h"
#include "msg_queue.h"
#include "req_resp.h"

struct msq_mgr {
	struct list_head queues[MSQ_TYPE_CNT];
};

static struct msq_mgr msq_mgr;

/*
 * Message queue implemented by ipc.
 */
int send_msg_ipc(struct msg_queue *queue, struct msgbuf *msg);
int recv_msg_ipc(struct msg_queue *queue, struct msgbuf *msg);
struct msg_queue *create_msg_queue_ipc(char *name);
struct msg_queue *get_msg_queue_ipc(char *name);
void delete_msg_queue_ipc(char *name);

/*
 * Message queue implemented by file.
 */
int send_msg_file(struct msg_queue *queue, struct msgbuf *msg);
int recv_msg_file(struct msg_queue *queue, struct msgbuf *msg);
struct msg_queue *create_msg_queue_file(char *name);
struct msg_queue *get_msg_queue_file(char *name);
void delete_msg_queue_file(char *name);

/*
 * Message queue implemented by net.
 */
int send_msg_net(struct msg_queue *queue, struct msgbuf *msg);
int recv_msg_net(struct msg_queue *queue, struct msgbuf *msg);
struct msg_queue *create_msg_queue_net(char *name);
struct msg_queue *get_msg_queue_net(char *name);
void delete_msg_queue_net(char *name);

/*
 * Message queue operations for ipc msq.
 */
struct msq_ops msq_ipc = {
	.send_msg = send_msg_ipc,
	.recv_msg = recv_msg_ipc,
};

/*
 * Message queue operations for file msq.
 */
struct msq_ops msq_file = {
	.send_msg = send_msg_file,
	.recv_msg = recv_msg_file,
};

/*
 * Message queue operations for net msq.
 */
struct msq_ops msq_net = {
	.send_msg = send_msg_net,
	.recv_msg = recv_msg_net,
};

static void __attribute__((constructor)) msq_mgr_init(void)
{
	int i = 0;

	for (i = 0; i < MSQ_TYPE_CNT; ++i)
		INIT_LIST_HEAD(&msq_mgr.queues[i]);
}

int send_msg_ipc(struct msg_queue *queue, struct msgbuf *msg)
{
	return 0;
}

int recv_msg_ipc(struct msg_queue *queue, struct msgbuf *msg)
{
	return 0;
}

struct msg_queue *create_msg_queue_ipc(char *name)
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
	queue->ops = &msq_ipc;

	return queue;
}

struct msg_queue *get_msg_queue_ipc(char *name)
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

void delete_msg_queue_ipc(char *name)
{
	struct msg_queue *queue;

	queue = get_msg_queue_ipc(name);
	if (queue == NULL) {
		return;
	}

	msgctl(queue->msqid, IPC_RMID, NULL);

	free(queue);
}

int send_msg_file(struct msg_queue *queue, struct msgbuf *msg)
{
	int ret;

	if (queue->fd == -1)
		queue->fd = open(MSG_QUEUE_FIFO_FILE, O_CREAT | O_WRONLY);

	if (queue->fd == -1) {
		perror("open fifo for write failed");
		printf("%s:%d, cannot open %s\n", __func__, __LINE__, MSG_QUEUE_FIFO_FILE);
		return -1;
	}

	ret = write(queue->fd, msg, sizeof(struct msgbuf));

	if (ret < sizeof(struct msgbuf))
		return -1;

	return 0;
}

int recv_msg_file(struct msg_queue *queue, struct msgbuf *msg)
{
	int ret;

	if (queue->fd == -1)
		queue->fd = open(MSG_QUEUE_FIFO_FILE, O_CREAT | O_RDONLY);

	if (queue->fd == -1) {
		perror("open fifo for read failed");
		printf("%s:%d, cannot open %s\n", __func__, __LINE__, MSG_QUEUE_FIFO_FILE);
		return -1;
	}

	ret = read(queue->fd, msg, sizeof(struct msgbuf));

	if (ret < sizeof(struct msgbuf))
		return -1;

	return 0;
}

void fifo_close_action(int sig)
{
	sleep(1);
}

struct msg_queue *create_msg_queue_file(char *name)
{
	struct msg_queue *queue;

	signal(SIGPIPE, fifo_close_action);

	queue = malloc(sizeof(struct msg_queue));
	if (queue == NULL)
		return NULL;

	bzero(queue, sizeof(struct msg_queue));

	strncpy(queue->name, name, MSQ_NAME_LEN - 1);
	queue->type = MSQ_TYPE_FILE;
	queue->msqid = -1;
	queue->fd = -1;
	queue->sock = -1;
	queue->ops = &msq_file;
	INIT_LIST_HEAD(&queue->list);

	return queue;
}

struct msg_queue *get_msg_queue_file(char *name)
{
	return NULL;
}

void delete_msg_queue_file(char *name)
{

}

int send_msg_net(struct msg_queue *queue, struct msgbuf *msg)
{
	return 0;
}

int recv_msg_net(struct msg_queue *queue, struct msgbuf *msg)
{
	return 0;
}

struct msg_queue *create_msg_queue_net(char *name)
{
	return NULL;
}

struct msg_queue *get_msg_queue_net(char *name)
{
	return NULL;
}

void delete_msg_queue_net(char *name)
{

}

int send_msg(struct msg_queue *queue, struct msgbuf *msg)
{
	int ret;

	if (queue == NULL || msg == NULL)
		return -1;

	ret = queue->ops->send_msg(queue, msg);

	return ret;
}

int recv_msg(struct msg_queue *queue, struct msgbuf *msg)
{

	int ret;

	if (queue == NULL || msg == NULL)
		return -1;

	ret = queue->ops->recv_msg(queue, msg);

	return ret;
}

struct msg_queue *create_msg_queue(char *name, enum msq_type type)
{
	struct list_head *head;
	struct msg_queue *queue;

	if (name == NULL)
		return NULL;

	if (!(type >= 0 && type < MSQ_TYPE_CNT))
		return NULL;

	if (get_msg_queue(name, type) != NULL)
		return NULL;

	head = &msq_mgr.queues[type];

	switch (type) {
	case MSQ_TYPE_IPC:
		queue = create_msg_queue_ipc(name);
		break;
	case MSQ_TYPE_FILE:
		queue = create_msg_queue_file(name);
		break;
	case MSQ_TYPE_NET:
		queue = create_msg_queue_net(name);
		break;
	default:
		queue = NULL;
		break;
	}

	if (queue == NULL)
		return NULL;

	list_add(&queue->list, head);

	return queue;
}

struct msg_queue *get_msg_queue(char *name, enum msq_type type)
{
	struct list_head *head;
	struct msg_queue *queue;
	bool found = false;

	if (name == NULL)
		return NULL;

	if (!(type >= 0 && type < MSQ_TYPE_CNT))
		return NULL;

	head = &msq_mgr.queues[type];

	list_for_each_entry(queue, head, list) {
		if (strcmp(queue->name, name) == 0) {
			found = true;
			break;
		}
	}

	if (found)
		return queue;

	return NULL;
}

void delete_msg_queue(struct msg_queue *queue)
{
}

