#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "md5.h"

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s str|file\n", argv[0]);
		return -1;
	}

	char str[32];
	char md5[33];
	//char *md5_print;

	if (file_is_existed(argv[1])) {
		printf("md5 file: %s\n", argv[1]);
		md5_file(argv[1], md5);	
	} else {
		strcpy(str, argv[1]);
		strcat(str, "\n");
		printf("md5 str: %s\n", str);
		md5_str(str, strlen(str), md5);
	}

	//md5_print = md5_printable_str(md5);
	//printf("%s\n", md5_print);
	printf("%s\n", md5);

	//free(md5_print);

	return 0;
}
