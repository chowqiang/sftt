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

#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include "connect.h"

int make_connect(char *ip, int port)
{
	int ret, sock;
	struct sockaddr_in serv_addr;

	printf("%s:%d, ip=%s, port=%d\n", __func__, __LINE__, ip, port);
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		perror("create socket failed");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(port);

	ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		perror("connect failed");
		return -1;
	}

	return sock;
}
