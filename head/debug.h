#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdbool.h>
#include <stdio.h>

#define DEBUG(expression)	\
	do {	\
		if (debug_print_enable()) {	\
			printf(expression);	\
		}	\
	} while (false)

bool debug_print_enable(void) {
#ifdef DEBUG_ENABLE
	return true;
#endif
	return false;
}

#endif
