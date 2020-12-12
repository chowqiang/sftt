#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdbool.h>
#include <stdio.h>

#define DEBUG_DEBUG			0x0000
#define	DEBUG_INFO			0x1001
#define	DEBUG_WARN			0x1002
#define DEBUG_ERROR			0x1004

#define DEBUG_PRINT_MASK	0x1000

static inline char *get_level_desc(int print_level) {
	switch (print_level) {
	case DEBUG_DEBUG:
		return "DEBUG";
	case DEBUG_INFO:
		return "INFO";
	case DEBUG_WARN:
		return "WARN";
	case DEBUG_ERROR:
		return "ERROR";
	}

	return "";
}

#define debug_print(print_level, fmt, ...)		\
	if (print_level & DEBUG_PRINT_MASK) {		\
		printf("[%s](%s|%d)"fmt,				\
			get_level_desc(print_level),		\
			__func__,							\
			__LINE__,							\
			##__VA_ARGS__);						\
	}

#if defined(DEBUG_ENABLE)
	#define DEBUG(Expression)	\
		do {					\
			debug_print Expression \
		} while (false)
#else
	#define DEBUG(Expression)
#endif

#endif
