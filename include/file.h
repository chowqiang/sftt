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

#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>
#include "req_resp.h"

#define FILE_TYPE_UNKNOWN	0
#define FILE_TYPE_FILE		1
#define FILE_TYPE_DIR		2

/*
 * Use absolute path and relative path to represent a path.
 */
struct path_entry {
	char abs_path[FILE_NAME_MAX_LEN]; /* absolute path */
	char rel_path[FILE_NAME_MAX_LEN]; /* relative path */
};

struct path_entry_list {
	struct path_entry entry;
	struct path_entry_list *next;
};

#define IS_DIR(type)	(type == FILE_TYPE_DIR)
#define IS_FILE(type)	(type == FILE_TYPE_FILE)

#define FILE_TYPE_NAME(type)          \
       ({                             \
	const char *__name;           \
	switch (type) {               \
	case FILE_TYPE_UNKNOWN:       \
		__name = "unknown";   \
		break;                \
	case FILE_TYPE_FILE:          \
		__name = "file";      \
		break;                \
	case FILE_TYPE_DIR:           \
		__name = "dir";       \
		break;                \
	default:                      \
		__name = "exception"; \
		break;                \
	 }                            \
	__name;                       \
	})

size_t file_size(char *filename);

unsigned char *file_get_contents(char *path, size_t *length);
size_t file_put_contents(char *path, unsigned char *text, size_t length); 

bool file_existed(char *path);
int is_dir(char *file_name);
int is_file(char *file_name);
bool same_file(char *path, char *md5);

char *path_join(char *dir, char *fname);

struct dlist *get_top_file_list(char *dir);
struct dlist *get_all_file_list(char *dir);

struct path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix);
struct path_entry *get_path_entry(char *path, char *pwd);

struct dlist *get_path_entry_list(char *path, char *pwd);
struct path_entry *get_file_path_entry(char *file_name);
struct path_entry *get_dir_path_entry_array(char *file_name, char *prefix, int *pcnt);

mode_t file_mode(char *path);
int set_file_mode(char *path, mode_t mode);

int create_new_file(char *fname, mode_t mode);
int create_temp_file(char *buf, char *prefix);

bool is_absolute_path(char *path);
bool is_relative_path(char *path);

char *get_basename(char *path);
#endif
