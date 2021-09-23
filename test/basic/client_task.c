#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

extern char *optarg;
extern int optind;
extern int optopt;

int sock = -1;
bool run = true;

void client_task_exit(int sig)
{
	if (sock == -1) {
		return;
	}

	run = false;
	close(sock);
}

void *task_handler(void *arg)
{
	int conn_fd;
	unsigned char hello[] = "hi, client, welcome!";

	if (sock == -1) {
		return (void *)-1;
	}

	while (run) {
		conn_fd = accept(sock, (struct sockaddr *)NULL, NULL);
		if (conn_fd == -1) {
			usleep(100 * 1000);
			continue;
		}
		printf("a client is connected!\n");
		send(conn_fd, hello, sizeof(hello), 0);
		usleep(100 * 1000);
		close(conn_fd);
	}

	return NULL;
}

pthread_t init_client_task(int *port)
{
	pthread_t tid;
	struct sockaddr_in taskaddr;
	int len;

	signal(SIGINT, client_task_exit);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("create socket failed");
		return -1;
	}

	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
		perror("set sockfd to non-block failed");
		return -1;
	}

	memset(&taskaddr, 0, sizeof(taskaddr));
	taskaddr.sin_family = AF_INET;
	taskaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	taskaddr.sin_port = 0;

	if (bind(sock, (struct sockaddr *)&taskaddr, sizeof(taskaddr)) == -1) {
		perror("bind socket error");
		return -1;
	}

	if (listen(sock, 10) == -1) {
		perror("listen socket error");
		return -1;
	}

	len = sizeof(struct sockaddr_in);
	if (getsockname(sock, (struct sockaddr *)&taskaddr, (socklen_t *)&len) == -1) {
		perror("getsockname error");
		return -1;
	}

	*port = ntohs(taskaddr.sin_port);

	if (pthread_create(&tid, NULL, task_handler, NULL) == -1) {
		perror("pthread create error");
		return -1;
	}

	return tid;
}

enum run_mode {
	CLIENT,
	SERVER,
	UNKNOWN,
};

int run_client(int port)
{
	int ret, _sock;
	struct sockaddr_in taskaddr;
	unsigned char buf[128];

	if ((_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("create socket failed");
		return -1;
	}

	memset(&taskaddr, 0, sizeof(taskaddr));

	taskaddr.sin_family = AF_INET;
	taskaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	taskaddr.sin_port = htons(port);

	if (connect(_sock, (struct sockaddr *)&taskaddr, sizeof(taskaddr)) == -1) {
		perror("connect failed");
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	ret = recv(_sock, buf, sizeof(buf), 0);
	if (ret == -1) {
		perror("recv failed");
		return -1;
	}

	printf("server say: %s\n", buf);
	printf("bye-bye\n");

	return 0;
}

int run_server(void)
{
	int port = -1;
	pthread_t tid;
	void *value;

	if ((tid = init_client_task(&port)) == -1) {
		printf("init client task failed!\n");
		return -1;
	}

	printf("task is listening on %d\n", port);

	getchar();
	kill(getpid(), SIGINT);

	pthread_join(tid, &value);

	return 0;
}

void client_task_usage(int code)
{
	printf("Usage: client_task [-s|-c] [-p]\n");

	exit(code);
}

int main(int argc, char *argv[])
{
	int ret;
	int port = -1;
	int run_mode = UNKNOWN;
	char ch;

	while ((ch = getopt(argc, argv, "scp:")) != -1) {
		switch (ch) {
		case 'p':
			port = atoi(optarg);
			if (!(port > 0 && port < 65536)) {
				printf("port is invalid!\n");
				return -1;
			}
			break;
		case 's':
			if (run_mode != UNKNOWN) {
				printf("cannot both run on client and server at the same time!\n");
				return -1;
			}
			run_mode = SERVER;
			break;
		case 'c':
			if (run_mode != UNKNOWN) {
				printf("cannot both run on client and server at the same time!\n");
				return -1;
			}
			run_mode = CLIENT;
			break;
		case '?':
		default:
			printf("unknown args!\n");
			client_task_usage(-1);
		}
	}

	if (run_mode == UNKNOWN) {
		printf("please specify the run mode (client or server?)\n");
		return -1;
	}

	if (run_mode == SERVER && port != -1) {
		printf("cannot specify port when run on server!\n");
		return -1;
	}

	if (run_mode == CLIENT && port == -1) {
		printf("please specify the port when run on client!\n");
		return -1;
	}

	switch (run_mode) {
	case CLIENT:
		ret = run_client(port);
		break;
	case SERVER:
		ret = run_server();
		break;
	default:
		printf("unknown run mode!\n");
		return -1;
	}

	return ret;
}
