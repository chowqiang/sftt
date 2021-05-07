#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <fcntl.h>
#include "endpoint.h"

int get_random_port(){
	time_t t;
	int port;

	t = time(NULL) / UPDATE_THRESHOLD;
	srand((unsigned)(t));
	port = rand() % (65535 - 1025) + 1025;
	//printf("time is %d\n", port);

	return port;
}

int make_client(void)
{
	return 0;
}

int make_server(int port)
{
	int	sockfd;
	struct sockaddr_in serveraddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("create socket filed");
		return -1;
	}

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
		perror("set sockfd to non-block failed");
		return -1;
	}

	memset(&serveraddr, 0 ,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		perror("bind socket error");
		return -1;
	}

	if (listen(sockfd, 10) == -1){
		perror("listen socket error");
		return -1;
	}

	return sockfd;
}
