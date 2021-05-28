#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdbool.h>

struct user_cmd {
	const char *name;
	int argc;
	char **argv;
};

struct cmd_handler {
	const char *name;
	int (*fn)(void *obj, int argc, char *argv[], bool *argv_check);
	const char *help;
	void (*usage)(void);
};

#endif

