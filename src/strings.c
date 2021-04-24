#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mem_pool.h"
#include "sftt_strings.h"

extern struct mem_pool *g_mp;

int init_sds(struct sds *str) {
	if (str == NULL) {
		return -1;
	}	
	
	int init_size = 8;
	char *tmp = (char *)mp_malloc(g_mp, sizeof(char) * init_size);
	if (tmp == NULL) {
		str->buf = NULL;
		str->size = 0;
		str->len = 0;

		return -1;
	} 
	memset(tmp, 0, init_size);
	str->buf = tmp;
	str->size = init_size;
	str->len = 0;

	return 0;
}

int sds_append_char(struct sds *str, char c) {
	if (str == NULL) {
		return -1;
	}

	int ret = 0;
	if (str->size == 0) {
		ret = init_sds(str);
		if (ret == -1) {
			return -1;
		}
		goto RET;
	}

	if (str->len < str->size - 1) {
		goto RET;
	}
	
	int goal = 0;
	if (str->size < MAX_SDS_AUTO_GROW_SIZE) {
		goal = (str->size != 0 ? str->size : 8) * 2;
	} else {
		goal = str->size + 16;
	}

	char *tmp = (char *)realloc(str->buf, sizeof(char) * goal);
	if (tmp == NULL) {
		return -1;
	}
	memset(&str->buf[str->size], 0, goal - str->size);
	str->buf = tmp;
	str->size = goal;
RET:
	str->buf[str->len] = c;
	str->len++;

	return 0;
}

struct strings *create_strings(void) {
	struct strings *ss = (struct strings *)mp_malloc(g_mp, sizeof(struct strings));
	if (ss == NULL) {
		return NULL;
	}

	int init_cap = 4;
	ss->str_arr = (struct sds *)mp_malloc(g_mp, sizeof(struct sds) * init_cap);
	if (ss->str_arr == NULL) {
		mp_free(g_mp, ss);
		return NULL;
	} 
	int i = 0;
	for (i = 0; i < init_cap; ++i) {
		init_sds(&ss->str_arr[i]);
	}
	ss->cap = init_cap;
	ss->num = 0;

	return ss;
}

int add_string(struct strings *ss) {
	if (ss == NULL) {
		return -1;
	}

	if (ss->num < ss->cap) {
		ss->num++;
		return 0;
	}

	int goal = 0;
	if (ss->cap < MAX_SDS_AUTO_GROW_SIZE) {
		goal = (ss->cap != 0 ? ss->cap : 4) * 2;	
	} else {
		goal = ss->cap + 10;
	}

	struct sds *tmp = (struct sds *)realloc(ss->str_arr, sizeof(struct sds) * goal);
	if (tmp == NULL) {
		return -1;
	}
	ss->str_arr = tmp;

	int i = 0, ret = 0;
	for (i = ss->cap; i < goal; ++i) {
		ret = init_sds(&ss->str_arr[i]);
		if (ret == -1) {
			break;
		}
	}
	ss->cap = goal;
	ss->num++;

	return 0;
}

int append_char(struct strings *ss, int index, char c) {
	if (ss == NULL || ss->num <= index) {
		return -1;
	}

	return sds_append_char(&ss->str_arr[index], c);
}

int get_string_num(struct strings *ss) {
	if (ss == NULL) {
		return -1;
	}

	return ss->num;
}

const char *get_string(struct strings *ss, int index) {
	if (ss == NULL || ss->num <= index) {
		return NULL;
	}

	return (const char *)ss->str_arr[index].buf;
}

void free_strings(struct strings **ss) {
	if (ss == NULL || *ss == NULL) {
		return ;
	}
	int i = 0;
	for (i = 0; i < (*ss)->cap; ++i) {
		if ((*ss)->str_arr[i].size != 0) {
			mp_free(g_mp, (*ss)->str_arr[i].buf);
		}
	}
	mp_free(g_mp, (*ss)->str_arr);
	mp_free(g_mp, *ss);
	*ss = NULL;
}

int strings_test(void) {
	struct strings *ss = create_strings();
	if (ss == NULL) {
		return -1;
	}	
	add_string(ss);
	append_char(ss, 0, 'h');
	append_char(ss, 0, 'e');
	append_char(ss, 0, 'l');
	append_char(ss, 0, 'l');
	append_char(ss, 0, 'o');

	const char *tmp = get_string(ss, 0);
	printf("%s\n", tmp);

	return 0;
}
