#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "connect.h"
#include "msq_thread.h"
#include "net_trans.h"
#include "packet.h"

void *msq_main_loop(void *arg)
{
	int sockfd;
	int connect_fd;
	int ret;
	struct server_context *ctx;
	struct sftt_packet *req;

	ctx = (struct server_context *)arg;
	sockfd = ctx->sock;

	req = malloc_sftt_packet();
	if (!req) {
		printf("cannot allocate resources from memory pool!\n");
		return NULL;
	}

	for (;;) {
		connect_fd = accept(sockfd, (struct sockaddr *)NULL, NULL);
		if (connect_fd == -1) {
			usleep(100 * 1000);
			continue;
		}

		ret = recv_sftt_packet(connect_fd, req);
		if (ret == -1) {
			printf("%s:%d, recv failed\n", __func__, __LINE__);
			continue;
		}

		if (ret == 0) {
			printf("%s:%d, recv length equals to zero\n", __func__, __LINE__);
			continue;
		}

		switch (req->type) {
		case PACKET_TYPE_WRITE_MSG_REQ:
			break;
		case PACKET_TYPE_READ_MSG_REQ:
			break;
		}
	}
}
