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

#ifndef _SHM_SPACE_H_
#define _SHM_SPACE_H_

#include <stdbool.h>

#define SHM_MAX_SECTION_NR	8

#define SHM_SPACE_SIZE	(1024 * 1024)

#define SHM_KEY_NAME_LEN	128
#define SHM_SECTION_NAME_LEN	32

struct shm_section {
	char name[SHM_SECTION_NAME_LEN];
	bool valid;
	void *start;
	int size;
};

struct shm_space {
	char name[SHM_KEY_NAME_LEN];
	int nr;
	void *start;
	void *end;
	struct shm_section sections[SHM_MAX_SECTION_NR];
};

struct shm_space *shm_space_construct(char *name, int size);
void shm_space_destruct(struct shm_space *ptr);
int shm_space_create_section(struct shm_space *ptr, char *name, int size);
int shm_space_dump_section(struct shm_space *ptr, char *name, void *data);
int shm_space_delete_section(struct shm_space *ptr, char *name);
struct shm_space *get_shm_space(char *name);
int shm_space_get_section_content(struct shm_space *ptr, char *name, char *data);
struct shm_section *shm_space_get_section(struct shm_space *ptr, char *name);

#endif
