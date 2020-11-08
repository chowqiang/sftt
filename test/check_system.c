#include <stdio.h>

int check_system(void)
{
	int i = 0x12345678;
	char *c = (char *)&i;
	return ((c[0] == 0x78) && (c[1] == 0x56) && (c[2] == 0x34) && (c[3] == 0x12));
}

int main(void) {
	if (check_system()) {
		printf("system is big end!\n");
	} else {
		printf("system is little end!\n");
	}
	return 0;
}
