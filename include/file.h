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

#define FILE_TYPE_UNKNOWN	0
#define FILE_TYPE_FILE		1
#define FILE_TYPE_DIR		2

size_t file_size(char *filename);

unsigned char *file_get_contents(char *path, size_t *length);

size_t file_put_contents(char *path, unsigned char *text, size_t length); 

bool file_is_existed(char *path);

int is_dir(char *file_name);

int is_file(char *file_name);

struct dlist *get_top_file_list(char *dir);

struct dlist *get_all_file_list(char *dir);

#endif
