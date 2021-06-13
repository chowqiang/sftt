#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "endpoint.h"
#include "net_trans.h"
#include "user.h"
#include "utils.h"

#define PROG	"net_trans"

void help(void)
{
	printf("Usage: \t" PROG " -c port name\n"
		"\t" PROG " -s\n");
	exit(-1);
}

int run_client(int port, char *user_name)
{
	int sock;
	int ret;
	char *host = "127.0.0.1";
	struct sftt_packet *req_packet, *resp_packet;
	struct validate_req *req_info;
	struct validate_resp *resp_info;

	if (port == -1)
		port = get_random_port();

	sock = make_client(host, port);
	assert(sock != -1);

	req_packet = malloc_sftt_packet(VALIDATE_REQ_PACKET_MIN_LEN);
	if (!req_packet) {
		printf("allocate request packet failed!\n");
		return -1;
	}
	req_packet->type = PACKET_TYPE_VALIDATE_REQ;

	req_info = malloc(sizeof(struct validate_req));
	assert(req_info != NULL);

	strncpy(req_info->name, user_name, USER_NAME_MAX_LEN - 1);
	req_info->name_len = strlen(req_info->name);

	req_packet->obj = (void *)req_info;
	req_packet->block_size = VALIDATE_REQ_PACKET_MIN_LEN;

	ret = send_sftt_packet(sock, req_packet);
	if (ret == -1) {
		printf("send validate request failed!\n");
		return -1;
	}

	resp_packet = malloc_sftt_packet(VALIDATE_RESP_PACKET_MIN_LEN);
	if (!resp_packet) {
		printf("allocate response packet failed!\n");
		return -1;
	}

	ret = recv_sftt_packet(sock, resp_packet);
	if (ret == -1) {
		printf("recv packet failed!\n");
		return -1;
	}

	resp_info = (validate_resp *)resp_packet->obj;
	if (resp_info->status != UVS_PASS) {
		printf("validate status is not pass!\n");
		return -1;
	}

	printf("pass the validate\n"
		"status: %d\n"
		"user_name: %s\n"
		"uid: %d\n"
		"session_id: %s\n",
		resp_info->status, resp_info->name,
		resp_info->uid, resp_info->session_id);
}

int run_server(void)
{
	int ret, num;
	int port;
	int socket_fd, connect_fd;
	struct validate_req *req_info;
	struct validate_resp *resp_info;
	struct sftt_packet *req_packet, *resp_packet;
	struct user_base_info *user;
	char session_id[32];

	port = get_random_port();
	socket_fd = make_server(port);
	assert(ret != -1);
	printf("server is running on 127.0.0.1:%d ...\n", port);

	req_packet = malloc_sftt_packet(VALIDATE_REQ_PACKET_MIN_LEN);
	assert(req_packet != NULL);

	resp_packet = malloc_sftt_packet(VALIDATE_RESP_PACKET_MIN_LEN);
	assert(resp_packet != NULL);

	resp_info = malloc(sizeof(struct validate_resp));
	assert(resp_info != NULL);

	while (1) {
		if ((connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1) {
			continue;
		}
		printf("a client connected!\n");

		ret = recv_sftt_packet(connect_fd, req_packet);
		if (ret == -1) {
			printf("recv packet failed!\n");
			close(connect_fd);
			continue;
		}

		req_info = req_packet->obj;
		user = find_user_base_by_name(req_info->name);
		if (user == NULL) {
			printf("get user info by name failed!\n");
			resp_info->status = UVS_INVALID;
		} else {
			resp_info->status = UVS_PASS;
			resp_info->uid = user->uid;
			strcpy(resp_info->name, user->name);
			gen_session_id(session_id, 16);
			strcpy(resp_info->session_id, session_id);
		}

		resp_packet->type = PACKET_TYPE_VALIDATE_RSP;
		resp_packet->obj = resp_info;

		printf("to be sent ...\n");
		int ret = send_sftt_packet(connect_fd, resp_packet);
		if (ret == -1) {
			printf("send validate response failed!\n");
		}
		//close(connect_fd);
	}
}

int main(int argc, char *argv[])
{
	int port = -1;

	if (argc < 2)
		help();

	if (!strcmp(argv[1], "-c")) {
		if (argc != 4)
			help();
		port = atoi(argv[2]);
		run_client(port, argv[3]);
	} else if (!strcmp(argv[1], "-s")) {
		run_server();
	}
	
	return 0;
}
