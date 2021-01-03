#include <stdio.h>
#include <stdarg.h>

int mylog(int level, const char *fmt, ...) {
	char tmp[1024];

	va_list args;
	va_start(args, fmt);
	vsnprintf(tmp, 1024, fmt, args);
	va_end(args);

	printf("%d: %s\n", level, tmp);

	return 0;
}

int main(void) {
	mylog(1, "%s -- %d", "hello, world", 30);

	return 0;
}

