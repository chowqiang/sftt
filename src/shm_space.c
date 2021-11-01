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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm_space.h"

#define in_range(value, start, end)				\
	({							\
		unsigned long __value = (unsigned long)value;	\
		unsigned long __start = (unsigned long)start;	\
		unsigned long __end = (unsigned long)end;	\
								\
		(__start <= __value) && (__value >= __end);	\
	})

struct shm_space *shm_space_construct(char *name, int size)
{
	key_t key;
	int i, shmid, shmflag;
	struct shm_space *ptr;

	if (name == NULL || size <= 1024)
		return NULL;

	if (strlen(name) > SHM_KEY_NAME_LEN - 1)
		return NULL;

	if ((key = ftok(name, 'S')) == -1) {
		printf("Ftok failed! File \"%s\" is existed?\n", name);
		return NULL;
	}

	shmflag = IPC_CREAT | 0666;
	if ((shmid = shmget(key, size + sizeof(struct shm_space),
		shmflag)) == -1) {
		printf("Shmget failed! shmflag: 0x%0x\n", shmflag);
		return NULL;
	}

	ptr = shmat(shmid, NULL, 0);
	if (ptr == (void *)(-1)) {
		printf("Shmat failed!\n");
		return NULL;
	}

	strncpy(ptr->name, name, SHM_KEY_NAME_LEN - 1);
	ptr->nr = 0;
	ptr->start = ptr + sizeof(struct shm_space);
	ptr->end = ptr->start + size;

	for (i = 0; i < SHM_MAX_SECTION_NR; ++i) {
		ptr->sections[i].name[0] = 0;
		ptr->sections[i].valid = 0;
		ptr->sections[i].start = 0;
		ptr->sections[i].size = 0;
	}

	return ptr;
}

void shm_space_destruct(struct shm_space *ptr)
{
	shmdt(ptr);
}

bool shm_space_try_this_range(struct shm_space *ptr, void *start, void *end)
{
	int i;

	for (i = 0; i < SHM_MAX_SECTION_NR; ++i) {
		if (!ptr->sections[i].valid)
			continue;

		if (in_range(ptr->sections[i].start, start, end)
			|| in_range(ptr->sections[i].start +
				ptr->sections[i].size - 1, start, end))
			return false;
	}

	return true;
}

int shm_space_create_section(struct shm_space *ptr, char *name, int size)
{
	int i;
	void *start, *end;
	struct shm_section *section = NULL;

	if (ptr == NULL || name == NULL || size <= 0)
		return -1;

	if (strlen(name) >= SHM_SECTION_NAME_LEN)
		return -1;

	if (ptr->nr >= SHM_MAX_SECTION_NR)
		return -1;

	for (i = 0; i < SHM_MAX_SECTION_NR; ++i) {
		if (!ptr->sections[i].valid) {
			start = ptr->sections[i].start;
			end = start + size;
		} else {
			start = ptr->sections[i].start + size;
			end = start + size;
		}

		if (shm_space_try_this_range(ptr, start, end)) {
			section = &ptr->sections[ptr->nr++];
			break;
		}
	}

	if (section) {
		strncpy(section->name, name, SHM_SECTION_NAME_LEN);
		section->valid = 1;
		section->start = start;
		section->size = size;

		return 0;
	}

	return -1;
}

int shm_space_dump_section(struct shm_space *ptr, char *name, void *data)
{
	int i;
	struct shm_section *section;

	section = shm_space_get_section(ptr, name);
	if (section == NULL)
		return -1;

	memcpy(section->start, data, section->size);

	return 0;
}

int shm_space_delete_section(struct shm_space *ptr, char *name)
{
	int i;
	struct shm_section *section;

	section = shm_space_get_section(ptr, name);
	if (section == NULL)
		return -1;

	section->valid = 0;

	return 0;
}

struct shm_space *get_shm_space(char *name)
{
	key_t key;
	int shmid, shmflag;
	struct shm_space *ptr;

	if (name == NULL)
		return NULL;

	if ((key = ftok(name, 'S')) == -1) {
		printf("Ftok failed! File \"%s\" is existed?\n", name);
		return NULL;
	}

	shmflag = 0666;
	if ((shmid = shmget(key, 0, shmflag)) == -1) {
		printf("Shmget failed! shmflag: 0x%0x\n", shmflag);
		return NULL;
	}

	ptr = shmat(shmid, NULL, 0);
	if (ptr == (void *)(-1)) {
		printf("Shmat failed!\n");
		return NULL;
	}

	return ptr;
}

int shm_space_get_section_content(struct shm_space *ptr, char *name, char *data)
{
	struct shm_section *section = NULL;

	if (data == NULL)
		return -1;

	section = shm_space_get_section(ptr, name);
	if (section == NULL)
		return -1;

	memcpy(data, section->start, section->size);

	return 0;
}

struct shm_section *shm_space_get_section(struct shm_space *ptr, char *name)
{
	int i;
	struct shm_section *section = NULL;

	if (ptr == NULL || name == NULL)
		return NULL;

	for (i = 0; i < SHM_MAX_SECTION_NR; ++i) {
		if (!ptr->sections[i].valid)
			continue;

		if (strcmp(ptr->sections[i].name, name) == 0) {
			section = &ptr->sections[i];
			break;
		}
	}

	return section;
}
