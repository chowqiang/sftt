#include <stdio.h>
#include "strtox.h"

int main(void) {
	char *str[] = {"freq=1300", "memsize1=60", "memsize2=180"};
	int freq = 0;
	int memsize1 = 0;
	int memsize2 = 0;

	char **p = str;
	parse_even_earlier(&freq, "freq", *p);
	++p;
	parse_even_earlier(&memsize1, "memsize1", *p);
	++p;
	parse_even_earlier(&memsize2, "memsize2", *p);
	++p;

	printf("freq: %d, memsize1: %d, memsize2: %d\n", freq, memsize1, memsize2);

	return 0;
}
