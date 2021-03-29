#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdbool.h>

typedef struct {
	const char *name;
	int argc;
	char **argv;
} user_cmd_t;

typedef struct {
	const char *name;
	int (*fn)(int argc, char *argv[], bool *argv_check);
	const char *help;
	void (*usage)(void);
} cmd_handler_t;

#endif

