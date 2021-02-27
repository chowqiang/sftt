#ifndef _STRINGS_H_
#define _STRINGS_H_

#define MAX_SDS_AUTO_GROW_SIZE	1024

typedef struct {
	int len;
	int size;
	char *buf;	
} sds;

typedef struct {
	sds *str_arr;
	int cap;	
	int num;
} strings;

strings *create_strings(void);

int add_string(strings *ss);

int append_char(strings *ss, int index, char c);

int get_string_num(strings *ss);

const char *get_string(strings *ss, int index);

void free_strings(strings **ss); 

#endif
