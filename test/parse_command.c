#include <stdio.h>

int main(int argc, char **argv) {
	int i = 0;
	char cmd[1024];
	printf("argc: %d\n", argc);
	for (i = 0; i < argc; ++i) {
		printf("%s\n", argv[i]);
	}

	fgets(cmd, 1023, stdin);
	printf("%s\n", cmd);

	return 0;
}
