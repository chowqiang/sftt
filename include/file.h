#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>

size_t file_size(char *filename);

unsigned char *file_get_contents(char *path, size_t *length);

size_t file_put_contents(char *path, unsigned char *text, size_t length); 

bool file_is_existed(char *path);

#endif
