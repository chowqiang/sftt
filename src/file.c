#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "file.h"
#include "mem_pool.h"

size_t file_size(char *filename) {
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

unsigned char *file_get_contents(char *path, size_t *length)
{
	FILE *pfile = NULL;
	unsigned char *data = NULL;
	struct mem_pool *mp = get_singleton_mp();
 
	*length = file_size(path);
	data = (unsigned char *)mp_malloc(mp, (*length + 1) * sizeof(unsigned char));
	if (data == NULL) {
		return NULL;
	}

	pfile = fopen(path, "rb");
	if (pfile == NULL)
	{
		return NULL;
	}

	*length = fread(data, 1, *length, pfile);
	data[*length] = '\0';

	fclose(pfile);

	return data;
}

size_t file_put_contents(char *path, unsigned char *text, size_t length) {
	FILE *fp = fopen(path, "wb");
	if (fp == NULL) {
		return -1;
	}

	size_t cnt = fwrite(text, 1, length, fp);
	fclose(fp);

	return cnt; 
}

bool file_is_existed(char *path)
{
	if (path == NULL || strlen(path) == 0)
		return false;

	return access(path, F_OK) != -1;
}
