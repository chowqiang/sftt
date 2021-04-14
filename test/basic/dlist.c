#include <stdio.h>
#include "dlist.h"

void show_argv(int argc, char *argv[])
{
	int i;
	for (i = 0; i < argc - 1; ++i) {
		printf("%s ", argv[i]);
	}
	printf("%s\n", argv[i]);
}

int main(int argc, char *argv[])
{
	//printf("test file: %s\n", __FILE__);
	//show_argv(argc, argv);
	struct dlist *list = dlist_create(NULL);

	return -1;
}
