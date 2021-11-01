/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

	unsigned char str[32];
	unsigned char md5[33];
	//char *md5_print;

	if (file_existed(argv[1])) {
		printf("md5 file: %s\n", argv[1]);
		md5_file(argv[1], md5);	
	} else {
		strcpy((char *)str, argv[1]);
		strcat((char *)str, "\n");
		printf("md5 str: %s\n", (char *)str);
		md5_str(str, strlen((char *)str), md5);
	}

	//md5_print = md5_printable_str(md5);
	//printf("%s\n", md5_print);
	printf("%s\n", md5);

	//free(md5_print);

	return 0;
}
