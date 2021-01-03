#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "connect.h"

int make_connect(char *ip, int port) {
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(port);

	int ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		return -1;
	}

	return sock;
}
