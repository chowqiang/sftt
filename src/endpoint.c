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
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <fcntl.h>
#include "connect.h"
#include "endpoint.h"
#include "utils.h"

int get_random_port(void)
{
	time_t t;
	int port;

	/*
	 * cannot use srand(), because it depends on platform.
	 * t = time(NULL) / UPDATE_THRESHOLD;
	 * srand((unsigned)(t));
	 */
	t = get_ts() / UPDATE_THRESHOLD;
	port = t % (65535 - 1025) + 1025;

	return port;
}

int make_client(char *host, int port)
{
	return make_connect(host, port);
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
