#ifndef _OPTION_H_
#define _OPTION_H_

#define NO_ARG					0x0000
#define HAS_ARG					0x0001
#define OPT_ARG					0x0010

enum option_index {
	USER,
	HOST,
	PORT,
	PASSWORD,
	START,
	RESTART,
	STOP,
	ADDUSER,
	UNKNOWN
};

typedef struct {
	const char *name;
	enum option_index index;
	int flags;
} sftt_option;

char *next_arg(char *arg, char **param, char **value);

const sftt_option *lookup_opt(int argc, char **argv, char **optarg, int *optind, const sftt_option *sftt_opts);

#endif

