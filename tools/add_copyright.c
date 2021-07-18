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
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

const char *copy_right = "\
/*\n\
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.\n\
 *\n\
 * This program is free software; you can redistribute it and/or modify\n\
 * it under the terms of the GNU General Public License version 2 as\n\
 * published by the Free Software Foundation.\n\
 *\n\
 * This program is distributed in the hope that it will be useful,\n\
 * but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 * GNU General Public License for more details.\n\
 *\n\
 * You should have received a copy of the GNU General Public License\n\
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\
 */\n\n\
";

bool no_need_add_cr(const char *file)
{
	FILE *fp;
	char buf[4];
	int ret;

	fp = fopen(file, "r");
	if (fp == NULL) {
		printf("cannot open %s\n", file);
		goto failed;
	}

	memset(buf, 0, 4);
	if (fread(buf, 1, 2, fp) != 2) {
		printf("read %s failed!\n", file);
		goto failed;
	}	

	fclose(fp);

	return strcmp(buf, "/*") == 0;
	
failed:
	fclose(fp);
	return true;
}

size_t file_size(const char *filename) {
	FILE *pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		return 0;
	}

	fseek(pfile, 0, SEEK_END);
	size_t length = ftell(pfile);
	rewind(pfile);

	fclose(pfile);

	return length;
}

int add_cr_to_file(const char *file)
{
	FILE *fp;
	size_t size;
	char *buf;

	size = file_size(file);
	if (size <= 0)
		return -1;

	if ((fp = fopen(file, "r+")) == NULL)
		return -1;

	buf = malloc(sizeof(char) * (size + 1));
	if (buf == NULL) {
		fclose(fp);
		return -1;
	}

	if (fread(buf, 1, size, fp) != size) {
		fclose(fp);
		free(buf);
		return -1;
	}

	rewind(fp);

	fwrite(copy_right, 1, strlen(copy_right), fp);
	fwrite(buf, 1, size, fp);

	fclose(fp);
	free(buf);

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s file\n", argv[0]);
		return -1;
	}

	if (no_need_add_cr(argv[1])) {
		printf("Skip %s ... It seems that this file already "
			"has copy right info.\n", argv[1]);
		return 0;
	}

	if (add_cr_to_file(argv[1]) == -1) {
		printf("Skip: %s ... Add copy right failed\n", argv[1]);
		return 0;
	}

	return 0;
}
