#ifndef _COMMAND_H_
#define _COMMAND_H_

struct command {
	const char *name;
	int (*fn)(int argc, char *argv[]);
	const char *help;
	void (*usage)(void);
};

#endif

