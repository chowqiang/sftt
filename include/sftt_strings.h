#ifndef _SFTT_STRINGS_H_
#define _SFTT_STRINGS_H_

#define MAX_SDS_AUTO_GROW_SIZE	1024

struct sds {
	int len;
	int size;
	char *buf;	
};

struct strings {
	sds *str_arr;
	int cap;	
	int num;
};

struct strings *create_strings(void);

int add_string(struct strings *ss);

int append_char(struct strings *ss, int index, char c);

int get_string_num(struct strings *ss);

const char *get_string(struct strings *ss, int index);

void free_strings(struct strings **ss); 

#endif
