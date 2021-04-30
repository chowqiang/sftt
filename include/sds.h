#ifndef _SFTT_STRINGS_H_
#define _SFTT_STRINGS_H_

#define MAX_SDS_AUTO_GROW_SIZE	1024

struct sds {
	char *buf;
	int len;
	int size;
	struct pthread_mutex *mutex;
};

#if 0
struct flex_array *create_strings(void);

int add_string(struct strings *ss);

int append_char(struct strings *ss, int index, char c);

int get_string_num(struct strings *ss);

const char *get_string(struct strings *ss, int index);

void free_strings(struct strings **ss); 
#endif

struct sds *sds_construct(void);
void sds_destruct(struct sds *ptr);
int sds_add_char(struct sds *ptr, char c);
int sds_add_str(struct sds *ptr, char *str);

#endif
