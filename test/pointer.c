#include <stdio.h>

int main(void) {
	char arr[] = "hello, world";
	char *p = arr;
	char *q = p++;
	*(q++) = 'a';

	printf("%s\n", arr);

	return 0;
}
