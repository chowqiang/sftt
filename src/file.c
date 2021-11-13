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

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "debug.h"
#include "dlist.h"
#include "file.h"
#include "md5.h"
#include "mem_pool.h"
#include "utils.h"

extern struct mem_pool *g_mp;

/*
 * Get the size of file in bytes.
 */
size_t file_size(char *filename)
{
	FILE *pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		printf("%s:%d, get file size failed!\n", __func__, __LINE__);
		return 0;
	}

	fseek(pfile, 0, SEEK_END);
	size_t length = ftell(pfile);
	rewind(pfile);

	fclose(pfile);

	return length;
}

/*
 * Get the whole contents of file.
 */
unsigned char *file_get_contents(char *path, size_t *length)
{
	FILE *pfile = NULL;
	unsigned char *data = NULL;
	struct mem_pool *mp = get_singleton_mp();

	*length = file_size(path);
	data = (unsigned char *)mp_malloc(mp, __func__, (*length + 1) * sizeof(unsigned char));
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

/*
 * Write the contents into file.
 */
size_t file_put_contents(char *path, unsigned char *text, size_t length)
{
	FILE *fp = fopen(path, "wb");
	if (fp == NULL) {
		return -1;
	}

	size_t cnt = fwrite(text, 1, length, fp);
	fclose(fp);

	return cnt;
}

/*
 * Is file existed.
 */
bool file_existed(char *path)
{
	if (path == NULL || strlen(path) == 0)
		return false;

	return access(path, F_OK) != -1;
}

int is_dir(char *file_name)
{
	struct stat file_stat;
	stat(file_name, &file_stat);

	return S_ISDIR(file_stat.st_mode);
}

int is_file(char *file_name)
{
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

	path = mp_malloc(g_mp, __func__, strlen(dir) + strlen(fname) + 2);
	if (path == NULL)
		return NULL;

	pos = stpcpy(path, dir);
	pos = stpcpy(pos, "/");
	stpcpy(pos, fname);

	return path;
}

/*
 * Get all files by DFS and link them in dlist.
 */
struct dlist *get_all_file_list(char *dir)
{
	struct dlist *list;
	struct dlist *sub_list;
	DIR *dp;
	struct dirent *entry;
	char *name;
	char *rp;
	static int count = 0;

	if (!file_existed(dir) || !is_dir(dir))
		return NULL;

	list = dlist_create(FREE_MODE_MP_FREE);

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

		if (is_dir(rp)) {
			sub_list = get_all_file_list(rp);
			if (sub_list && !dlist_empty(sub_list))
				dlist_merge(list, sub_list);
		}
	}

	closedir(dp);

	return list;
}

/*
 * Get the files of the top level in dir.
 */
struct dlist *get_top_file_list(char *dir)
{
	struct dlist *list;
	DIR *dp;
	struct dirent *entry;
	char *name;

	if (!file_existed(dir) || !is_dir(dir))
		return NULL;

	list = dlist_create(FREE_MODE_MP_FREE);

	if ((dp = opendir(dir)) == NULL) {
		return NULL;
	}

	while ((entry = readdir(dp)) != NULL) {
		name = __strdup(entry->d_name);
		dlist_append(list, name);
	}

	closedir(dp);

	return list;
}

bool same_file(char *path, char *md5)
{
	unsigned char real_md5[MD5_STR_LEN];

	if (!file_existed(path))
		return false;

	md5_file(path, real_md5);

	return strcmp((char *)real_md5, md5) == 0;
}

struct path_entry *get_file_path_entry(char *file_name)
{
	struct path_entry *pe;

        pe = (struct path_entry *)mp_malloc(g_mp, __func__, sizeof(struct path_entry));
	if (pe == NULL) {
		return NULL;
	}

	realpath(file_name, pe->abs_path);
	char *p = basename(pe->abs_path);
	strcpy(pe->rel_path, p);

	return pe;
}

void free_path_entry_list(struct path_entry_list *head)
{
	struct path_entry_list *p = head, *q = head;
	while (p) {
		q = p->next;
		mp_free(g_mp, p);
		p = q;
	}
}

struct path_entry *get_dir_path_entry_array(char *file_name, char *prefix, int *pcnt)
{
	*pcnt = 0;
	struct path_entry_list *head = get_dir_path_entry_list(file_name, prefix);
	if (head == NULL) {
		return NULL;
	}

	int count = 0;
	struct path_entry_list *p = head;
	while (p) {
		++count;
		p = p->next;
	}

	struct path_entry *array = (struct path_entry *)mp_malloc(g_mp, __func__,
			sizeof(struct path_entry) * count);
	if (array == NULL) {
		free_path_entry_list(head);
		return NULL;
	}

	int i = 0;
	p = head;
	for (i = 0; i < count; ++i) {
		if (p == NULL) {
			printf("Error. Unexpected fatal when copy path entry!\n");
		}
		strcpy(array[i].abs_path, (p->entry).abs_path);
		strcpy(array[i].rel_path, (p->entry).rel_path);
		p = p->next;
	}

	*pcnt = count;

	free_path_entry_list(head);

	return array;
}

struct path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix)
{
	struct path_entry_list *head = NULL;
	struct path_entry_list *current_entry = NULL;
	struct path_entry_list *sub_list = NULL;

	char dir_abs_path[FILE_NAME_MAX_LEN];
	char dir_rel_path[FILE_NAME_MAX_LEN];
	char tmp_path[2 * FILE_NAME_MAX_LEN + 1];

	realpath(file_name, dir_abs_path);
	char *p = basename(dir_abs_path);
	sprintf(dir_rel_path, "%s/%s", prefix, p);

	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	if ((dp = opendir(file_name)) == NULL) {
		printf("Error. cannot open dir: %s\n", file_name);
		return NULL;
	}

	chdir(file_name);
	while ((entry = readdir(dp)) != NULL) {
		snprintf(tmp_path, sizeof(tmp_path), "%s/%s", dir_abs_path, entry->d_name);
		lstat(tmp_path, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}
			//printf("dir tmp path: %s\n", tmp_path);
			sub_list = get_dir_path_entry_list(tmp_path, dir_rel_path);
			if (sub_list == NULL) {
				continue;
			}

			if (current_entry == NULL) {
				current_entry = head = sub_list;
			} else {
				current_entry->next = sub_list;
			}

			while (current_entry->next) {
				current_entry = current_entry->next;
			}

		} else {
			struct path_entry_list *node =
				(struct path_entry_list *)mp_malloc(g_mp,
					__func__, sizeof(struct path_entry_list));
			if (node == NULL) {
				continue;
			}

			sprintf(node->entry.abs_path, "%s/%s", dir_abs_path, entry->d_name);
			sprintf(node->entry.rel_path, "%s/%s", dir_rel_path, entry->d_name);
			//printf("file: %s\n", node->entry.abs_path);
			node->next = NULL;

			if (current_entry == NULL) {
				current_entry = head = node;
			} else {
				current_entry->next = node;
				current_entry = node;
			}
		}
	}
	closedir(dp);

	return head;
}

bool is_absolute_path(char *path)
{
	return path && path[0] == '/';
}

bool is_relative_path(char *path)
{
	return path && path[0] != '/';
}

void set_path_entry(struct path_entry *entry, char *abs, char *rel)
{
	strncpy(entry->abs_path, abs, FILE_NAME_MAX_LEN - 1);
	strncpy(entry->rel_path, rel, FILE_NAME_MAX_LEN - 1);
}

int path_append(char *path, char *sub_path, int max_len)
{
	if (strlen(path) + strlen(sub_path) > max_len - 2)
		return -1;

	strcat(path, "/");
	strcat(path, sub_path);

	return 0;
}

struct path_entry *dup_path_entry(struct path_entry *src)
{
	struct path_entry *entry;

	entry = mp_malloc(g_mp, __func__, sizeof(struct path_entry));
	set_path_entry(entry, src->abs_path, src->rel_path);

	return entry;
}

struct dlist *__get_path_entry_list(struct path_entry *root)
{
	DIR *dp;
	struct dirent *item;
	struct path_entry *entry;
	struct dlist *list;
	struct dlist *sub_list;

	if (!is_dir(root->abs_path))
		return NULL;

	if ((dp = opendir(root->abs_path)) == NULL) {
		return NULL;
	}

	list = dlist_create(FREE_MODE_MP_FREE);

	dlist_append(list, root);

	while ((item = readdir(dp)) != NULL) {
		//printf("%s\n", item->d_name);
		if (strcmp(item->d_name, ".") == 0 ||
			strcmp(item->d_name, "..") == 0)
			continue;

		entry = dup_path_entry(root);
		path_append(entry->abs_path, item->d_name, FILE_NAME_MAX_LEN - 1);
		path_append(entry->rel_path, item->d_name, FILE_NAME_MAX_LEN - 1);

		dlist_append(list, entry);

		if (is_dir(entry->abs_path)) {
			sub_list = __get_path_entry_list(entry);
			if (sub_list && !dlist_empty(sub_list))
				dlist_merge(list, sub_list);
		}
	}

	closedir(dp);

	return list;
}

struct dlist *get_path_entry_list(char *path, char *pwd)
{
	struct path_entry *root = NULL;
	char *p, *rp, *base;

	if (!is_dir(path))
		return NULL;

	root = mp_malloc(g_mp, __func__, sizeof(struct path_entry));
	if (root == NULL) {
		DEBUG((DEBUG_ERROR, "alloc path_entry failed!\n"));
		return NULL;
	}

	if (is_absolute_path(path)) {
		rp = path;
	} else if (pwd) {
		rp = path_join(pwd, path);
	} else {
		rp = path;
		//return NULL;
	}

	if (!file_existed(rp)) {
		DEBUG((DEBUG_ERROR, "file not exists: %s\n", rp));
		return NULL;
	}

	base = basename(rp);
	set_path_entry(root, rp, base);

	return __get_path_entry_list(root);
}

struct path_entry *get_path_entry(char *path, char *pwd)
{
	char *p;
	char *rp;
	struct path_entry *entry = NULL;

	if (!file_existed(path)) {
		if (is_absolute_path(path) || pwd == NULL)
			return NULL;

		rp = path_join(pwd, path);
		assert(is_absolute_path(rp));

		if (!file_existed(rp))
			return NULL;

	} else {
		rp = path;
	}

	entry = mp_malloc(g_mp, __func__, sizeof(struct path_entry));
	if (entry == NULL)
		return NULL;

	p = basename(rp);
	set_path_entry(entry, rp, p);

	return entry;
}

mode_t file_mode(char *path)
{
	struct stat file_stat;
	stat(path, &file_stat);

	return file_stat.st_mode;
}

int set_file_mode(char *path, mode_t mode)
{
	int ret;

	ret = chmod(path, mode);
	if (ret) {
		perror("chmod failed");
		printf("%s:%d, chmod failed: %s, mode: 0x%0x\n",
			__func__, __LINE__, path, mode);
	}

	return ret;
}

int create_temp_file(char *buf, char *prefix)
{
	int fd;
	char template[64];

	if (strlen(prefix) >= 32)
		return -1;

	sprintf(template, "/tmp/%sXXXXXX", prefix);
	fd = mkstemp(template);
	if (fd == -1) {
		perror("create temp file failed");
		return -1;
	}

	close(fd);
	strcpy(buf, template);

	return 0;
}

int create_new_file(char *fname, mode_t mode)
{
	FILE *fp;

	fp = fopen(fname, "w+");
	if (fp == NULL) {
		printf("%s:%d, fopen file failed: %s\n", __func__, __LINE__, fname);
		return -1;
	}

	fclose(fp);

       	return set_file_mode(fname, mode);
}

char *get_basename(char *path)
{
	return basename(path);
}
