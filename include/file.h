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

struct dlist *get_file_list(char *dir);

#endif
