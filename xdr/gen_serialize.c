#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define DEBUG 0

#define SZ_1MB	(1024 * 1024)

struct st_list {
	char struct_name[1024];
	struct st_list *next;
};

static size_t file_size(char *filename) {
	FILE *pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		return 0;
	}

	fseek(pfile, 0, SEEK_END);
	size_t length = ftell(pfile);
	rewind(pfile);

	fclose(pfile);

	return length;
}

char *fetch_next_str(char **str)
{
	char *p, *q;

	if (str == NULL || *str == NULL)
		return NULL;

	p = *str;
	/* skip blank */
	while (*p && isspace(*p))
		++p;
	if (!*p)
		return NULL;

	q = p;
	while (*q && !isspace(*q))
		++q;
	if (*q) {
		*q = 0;
		*str = q + 1;
	} else {
		*str = q;
	}

	return p;
}

int find_char(char *str, char ch)
{
	int i = 0;

	while (str[i]) {
		if (str[i] == ch)
			return i;
		++i;
	}

	return -1;
}

int get_struct_list(char *xdr_file, struct st_list **phead)
{
	FILE *xdr_fp = NULL;
	char *content = NULL;
	char *p = NULL;
	char *key_word = NULL;
	char *struct_name = NULL;
	char *word = NULL;
	size_t size = 0;
	int ret = 0, len = 0, idx = 0;
	struct st_list *head = NULL, *node = NULL, *tail = NULL;

	if ((size = file_size(xdr_file)) > SZ_1MB) {
		printf("file %s too long, failed to hanlde\n", xdr_file);
		return -1;
	}
	//printf("xdr file size: %d\n", size);

	content = malloc(size + 1);
	if (!content) {
		printf("cannot alloc memory for xdr file content\n");
		return -1;
	}
	memset(content, 0, size + 1);

	xdr_fp = fopen(xdr_file, "r");
	assert(xdr_fp != NULL);

	ret = fread(content, 1, size, xdr_fp);
	assert(ret == size);
	fclose(xdr_fp);

	//printf("begin parse xdr file\n");
	p = content;
	key_word = fetch_next_str(&p);
#if DEBUG
	printf("key word: %s\n", key_word);
#endif
	while (key_word && strlen(key_word)) {
		if (strcmp(key_word, "struct") == 0) {
			struct_name = fetch_next_str(&p);
			if (struct_name == NULL)
				break;

			len = strlen(struct_name);
			if (len == 0)
				break;

			idx = find_char(struct_name, '{');
			if (idx != -1) {
				struct_name[idx] = 0;
				assert(strlen(struct_name) < 1024);

				//printf("find a struct: %s\n", struct_name);
				node = malloc(sizeof(struct st_list));
				assert(node != NULL);

				strcpy(node->struct_name, struct_name);
				node->next = NULL;

				if (head == NULL) {
					head = tail = node;
				} else {
					tail->next = node;
					tail = node;
				}

			} else {
				word = fetch_next_str(&p);
				if (word == NULL)
					break;

				if (word[0] == '{') {
					assert(strlen(struct_name) < 1024);
					//printf("find a struct: %s\n", struct_name);

					node = malloc(sizeof(struct st_list));
					assert(node != NULL);

					strcpy(node->struct_name, struct_name);
					node->next = NULL;

					if (head == NULL) {
						head = tail = node;
					} else {
						tail->next = node;
						tail = node;
					}
				}
			}
		}
		key_word = fetch_next_str(&p);
#if DEBUG
		if (key_word)
			printf("key word: %s\n", key_word);
#endif
	}

	//printf("end parse xdr file\n");
	free(content);

	*phead = head;

	return 0;
}

void show_st_list(struct st_list *head)
{
	struct st_list *p = head;

	printf("struct name list: ");
	while (p) {
		printf("%s ", p->struct_name);
		p = p->next;
	}
	printf("\n");
}

int gen_h_file(struct st_list *head, char *h_file)
{
	struct st_list *p = head;
	FILE *fp = NULL;
	assert(head != NULL && h_file != NULL);

	fp = fopen(h_file, "w");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open '%s'\n", h_file);
		return -1;
	}

	fprintf(fp, "/*\n * Automatically generated - do not edit\n */\n\n");
	fprintf(fp, "#ifndef _SERIALIZE_H_\n");
	fprintf(fp, "#define _SERIALIZE_H_\n\n");

	fprintf(fp, "#include <stdbool.h>\n\n");
	while (p) {
		fprintf(fp, "bool %s_encode(void *req, unsigned char **buf, int *len);\n\n", p->struct_name);
		fprintf(fp, "bool %s_decode(unsigned char *buf, int len, void **req);\n\n", p->struct_name);
		p = p->next;
	}
	fprintf(fp, "#endif\n");

	fclose(fp);

	return 0;
}

void output_encode(FILE *fp, char *struct_name)
{
	fprintf(fp, "bool %s_encode(void *req, unsigned char **buf, int *len)\n", struct_name);
	fprintf(fp, "{\n");
	fprintf(fp, "\tadd_log(LOG_INFO, \"%%s: in\", __func__);\n");
	fprintf(fp, "\tsize_t size = 0;\n");
	fprintf(fp, "\tFILE *fp = open_memstream((char **)buf, &size);\n\n");
	fprintf(fp, "\tXDR xdr;\n");
	fprintf(fp, "\txdrstdio_create(&xdr, fp, XDR_ENCODE);\n\n");
	fprintf(fp, "\tint ret = xdr_%s(&xdr, (struct %s*)req);\n\n", struct_name, struct_name);
	fprintf(fp, "\tfclose(fp);\n");
	fprintf(fp, "\t*len = size;\n");
	fprintf(fp, "\tadd_log(LOG_INFO, \"%%s: encode ret=%%d, encode_len=%%d\", __func__, ret, *len);\n");
	fprintf(fp, "\tadd_log(LOG_INFO, \"%%s: out\", __func__);\n\n");
	fprintf(fp, "\treturn ret;\n");

	fprintf(fp, "}\n\n");
}

void output_decode(FILE *fp, char *struct_name)
{
	fprintf(fp, "bool %s_decode(unsigned char *buf, int len, void **req)\n", struct_name);
	fprintf(fp, "{\n");
	fprintf(fp, "\tadd_log(LOG_INFO, \"%%s: in\", __func__);\n");
	fprintf(fp, "\tstruct %s *_req = (struct %s *)mp_malloc(g_mp, sizeof(struct %s));\n\n", struct_name, struct_name, struct_name);
	fprintf(fp, "\tFILE *fp = fmemopen(buf, len, \"r\");\n\n");
	fprintf(fp, "\tXDR xdr;\n");
	fprintf(fp, "\txdrstdio_create(&xdr, fp, XDR_DECODE);\n\n");
	fprintf(fp, "\tint ret = xdr_%s(&xdr, _req);\n", struct_name);
	fprintf(fp, "\tfclose(fp);\n\n");
	fprintf(fp, "\t*req = _req;\n");
	fprintf(fp, "\tadd_log(LOG_INFO, \"%%s: decode ret=%%d\", __func__, ret);\n");
	fprintf(fp, "\tadd_log(LOG_INFO, \"%%s: out\", __func__);\n");
	fprintf(fp, "\treturn ret;\n");
	fprintf(fp, "}\n");
}

int gen_c_file(struct st_list *head, char *c_file)
{
	FILE *fp = NULL;
	struct st_list *p = head;
	assert(head != NULL && c_file != NULL);

	fp = fopen(c_file, "w");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open '%s'\n", c_file);
		return -1;
	}

	fprintf(fp, "/*\n * Automatically generated - do not edit\n */\n\n");
	fprintf(fp, "#include <stdbool.h>\n");
	fprintf(fp, "#include <stdio.h>\n");
	fprintf(fp, "#include \"log.h\"\n");
	fprintf(fp, "#include \"mem_pool.h\"\n");
	fprintf(fp, "#include \"req_rsp.h\"\n");
	fprintf(fp, "#include \"serialize.h\"\n");
	fprintf(fp, "#include \"xdr.h\"\n\n");
	fprintf(fp, "extern struct mem_pool *g_mp;\n\n");

	while (p) {
		output_encode(fp, p->struct_name);
		output_decode(fp, p->struct_name);
		p = p->next;
		if (p) {
			fprintf(fp, "\n");
		}
	}

	fclose(fp);

	return 0;
}

int gen_serialize(struct st_list *head, char *h_file, char *c_file)
{
	FILE *fp = NULL;

	if (gen_h_file(head, h_file) == -1) {
		printf("gen header file failed!\n");
		return -1;
	};

	if (gen_c_file(head, c_file) == -1) {
		printf("gen src file failed!\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct st_list *head = NULL;
	if (argc != 4) {
		printf("Usage: %s xdr_file h_file c_file\n", argv[0]);
		return -1;
	}

	if (get_struct_list(argv[1], &head) == -1) {
		printf("get struct list failed\n");
		return -1;
	}

	show_st_list(head);
	if (gen_serialize(head, argv[2], argv[3]) == -1) {
		printf("gen serialize failed\n");
		return -1;
	}

	return 0;
}
