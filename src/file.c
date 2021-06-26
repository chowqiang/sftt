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
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "dlist.h"
#include "file.h"
#include "mem_pool.h"

extern struct mem_pool *g_mp;

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

int is_dir(char *file_name) {
	struct stat file_stat;
	stat(file_name, &file_stat);

	return S_ISDIR(file_stat.st_mode);
}

int is_file(char *file_name) {
	struct stat file_stat;
	stat(file_name, &file_stat);

	return S_ISREG(file_stat.st_mode);
}

char *path_join(char *dir, char *fname)
{
	char *path = NULL;
	char *pos;

	if (dir == NULL || fname == NULL)
		return NULL;

	path = mp_malloc(g_mp, strlen(dir) + strlen(fname) + 2);
	if (path == NULL)
		return NULL;

	pos = stpcpy(path, dir);
	pos = stpcpy(pos, "/");
	stpcpy(pos, fname);

	return path;
}

struct dlist *get_all_file_list(char *dir)
{
	struct dlist *list;
	struct dlist *sub_list;
	DIR *dp;
	struct dirent *entry;
	char *name;
	char *rp;
	static int count = 0;

	if (!file_is_existed(dir) || !is_dir(dir))
		return NULL;

	list = dlist_create(NULL);

	if ((dp = opendir(dir)) == NULL) {
		return NULL;
	}

	while ((entry = readdir(dp)) != NULL) {
		//printf("%s\n", entry->d_name);
		if (strcmp(entry->d_name, ".") == 0 ||
			strcmp(entry->d_name, "..") == 0)
			continue;

		rp = path_join(dir, entry->d_name);
		dlist_append(list, rp);
#if 0
		count++;
		if (count > 1000)
			break;
		printf("%s\n", rp);
#endif

		if (is_dir(rp)) {
			sub_list = get_all_file_list(rp);
			if (sub_list && !dlist_empty(sub_list))
				dlist_merge(list, sub_list);
		}
	}

	closedir(dp);

	return list;
}

struct dlist *get_top_file_list(char *dir)
{
	struct dlist *list;
	DIR *dp;
	struct dirent *entry;
	char *name;

	if (!file_is_existed(dir) || !is_dir(dir))
		return NULL;

	list = dlist_create(NULL);

	if ((dp = opendir(dir)) == NULL) {
		return NULL;
	}

	while ((entry = readdir(dp)) != NULL) {
		name = strdup(entry->d_name);
		dlist_append(list, name);
	}

	closedir(dp);

	return list;
}
