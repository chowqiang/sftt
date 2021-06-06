#ifndef _SHM_SPACE_H_
#define _SHM_SPACE_H_

#include <stdbool.h>

#define SHM_MAX_SECTION	128

struct shm_section {
	char *name;
	bool valid;
	void *start;
	int size;
};

struct shm_space {
	char *name;
	int nr;
	void *start;
	void *end;
	struct shm_section sections[SHM_MAX_SECTION];
};

struct shm_space *shm_space_construct(char *name, int size);
void shm_space_destruct(struct shm_space *ptr);
int shm_space_create_section(struct shm_space *ptr, char *name, int size);
int shm_space_dump_section(struct shm_space *ptr, char *name, void *data);
int shm_space_delete_section(struct shm_space *ptr, char *name);

#endif
