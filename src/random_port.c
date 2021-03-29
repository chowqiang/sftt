#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "random_port.h"

int get_random_port(){
	time_t t;
	int port;

	t = time(NULL) / UPDATE_THRESHOLD;
	srand((unsigned)(t));
	port = rand() % (65535 - 1025) + 1025;
	//printf("time is %d\n", port);

	return port;
}



