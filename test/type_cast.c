#include <stdio.h>

void *a = 0;

void set(void *p) {
	a = p; 
	printf("a has been set to: %d\n", a);
}

void get(void **p) {
	if (p) {
		*p = a;
		printf("get value from a: %d\n", *p);
	}
}

int test(void) {
	char *s = "hello, world";
	printf("string s: %s\n", s);

	int data = 1;
	printf("save data: %d\n", data);
	set((void *)data);

	data = 2;
	printf("change data to: %d\n", data);

	get((void **)&data);
	printf("restore data to: %d\n", data);

	printf("string s: %s\n", s);

	return 0;
}

int test2(void) {
	char *s = "hello, world";
	int data = 1;

	char *c = (char *)&s;
 	printf("s addr: %p, s size: %d, s content: 0x%0x, s content's first byte: 0x%0x\n", &s, sizeof(s), s, *c);
	printf("data addr: %p, data size: %d, data content: %d\n", &data, sizeof(data), data);
	
	return 0;
}

int main(void) {
	test2();

	return 0; 
}
