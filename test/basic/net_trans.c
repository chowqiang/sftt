#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "endpoint.h"
#include "net_trans.h"

void help(char *prog)
{
	printf("Usage: %s -c|-s\n", prog);
	exit(-1);
}

void run_client(void)
{
	int ret;
	char *user_name = "root";

	ret = make_client();
	assert(ret == 0);

	struct sftt_packet *req = malloc_sftt_packet(VALIDATE_PACKET_MIN_LEN);
	if (!req) {
		printf("allocate request packet failed!\n");
		return ;
	}
	req->type = PACKET_TYPE_VALIDATE_REQ;

	struct validate_req v_req;
	
	char *tmp = strncpy(v_req.name, user_name, USER_NAME_MAX_LEN - 1);
	v_req.name_len = strlen(tmp);
}

void run_server(void)
{
	int ret;
	int port;
	int socket_fd, connect_fd;

	port = get_random_port();
	socket_fd = make_server(port);
	assert(ret != -1);

	while (1) {
		if ((connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1) {
	
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		help(argv[0]);

	if (!strcmp(argv[1], "-c")) {
		run_client();	
	} else if (!strcmp(argv[1], "-s")) {
		run_server();
	}
	
	return 0;
}
