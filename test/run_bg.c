#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

void sighandler(int signum) {
   printf("Caught signal %d, coming out...\n", signum);
}

int main(int argc, char **args) {

	printf("server is running on background ...\n");
	signal(SIGINT, sighandler);
	int ret = daemon(1, 1);
	if (ret != 0) {
		printf("server cannot running on background ...\n");
		exit(-1);
	}
	while (true) {
		sleep(1);
		printf("message from background ...\n");
	}

	return 0;
}
