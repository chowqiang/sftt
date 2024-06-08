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

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "list.h"

#define DEBUG 0

#define SZ_1MB	(1024 * 1024)

#define ST_NAME_LEN 64
#define ELEM_NAME_LEN 64

#define BUG() 		\
	do { 		\
		printf("%s:%d, unreachable!\n", __func__, __LINE__);	\
		assert(0);	\
	} while (0)

#define ELEM_TYPE_UNSIGNED	(1UL << 31)
#define ELEM_TYPE_CHAR		(1UL << 0)
#define ELEM_TYPE_SHORT		(1UL << 1)
#define ELEM_TYPE_INT		(1UL << 2)
#define ELEM_TYPE_STRUCT	(1UL << 3)

#define ELEM_TYPE_UNSIGNED_CHAR 	(ELEM_TYPE_UNSIGNED | ELEM_TYPE_CHAR)
#define ELEM_TYPE_UNSIGNED_SHORT	(ELEM_TYPE_UNSIGNED | ELEM_TYPE_SHORT)
#define ELEM_TYPE_UNSIGNED_INT		(ELEM_TYPE_UNSIGNED | ELEM_TYPE_INT)

#define DIMS_MAX_NUM	5

struct elem {
	char name[ELEM_NAME_LEN];
	unsigned int type;
	char st_name[ST_NAME_LEN];
	char *dims[DIMS_MAX_NUM];
	struct list_head list;
};

struct st {
	char name[ST_NAME_LEN];
	struct list_head elems;
	struct list_head list;
};

LIST_HEAD(st_list);

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

static bool endswith(char *src, char *foo)
{
	size_t src_len = 0, foo_len = 0;
	char *pos = NULL;

	if (src == NULL || foo == NULL)
		return false;

	src_len = strlen(src);
	foo_len = strlen(foo);
	if (src_len < foo_len)
		return false;

	if ((pos = strstr(src, foo)) == NULL)
		return false;

	return strlen(pos) == foo_len;
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

int parse_name_and_dims(struct elem *elem, char *p)
{
	char *tmp = NULL;
	int cnt = 0;

	tmp = strchr(p, '[');
	if (tmp == NULL) {
		strncpy(elem->name, p, ELEM_NAME_LEN - 1);
		return 0;
	}

	*tmp = 0;
	strncpy(elem->name, p, ELEM_NAME_LEN - 1);

	while (*p && tmp) {
		assert(cnt < DIMS_MAX_NUM);

		/* skip to '[' */
		p = tmp + 1;

		tmp = strchr(p, ']');
		assert(tmp);

		*tmp = 0;
		elem->dims[cnt++] = strdup(p);

		/* skip to ']' */
		p = tmp + 1;
		if (*p)
			tmp = strchr(p, '[');
	}

	return 0;
}

int parse_struct(struct st *st, char **str)
{
	char *p = NULL, *q = NULL;
	struct elem *elem = NULL;
	int ret = 0;

	q = *str;
	while (*q) {
		p = fetch_next_str(&q);
		assert(p);

		if (strcmp(p, "};") == 0)
			break;

		elem = malloc(sizeof(struct elem));
		assert(elem);

		memset(elem, 0, sizeof(struct elem));
		INIT_LIST_HEAD(&elem->list);

		if (strcmp(p, "unsigned") == 0) {
			elem->type = ELEM_TYPE_UNSIGNED;
			p = fetch_next_str(&q);
			assert(p);
		}

		if (strcmp(p, "char") == 0) {
			elem->type |= ELEM_TYPE_CHAR;

		} else if (strcmp(p, "short") == 0) {
			elem->type |= ELEM_TYPE_SHORT;

		} else if (strcmp(p, "int") == 0) {
			elem->type |= ELEM_TYPE_INT;

		} else if (strcmp(p, "struct") == 0) {
			if (elem->type) {
				printf("cannot use 'struct' as the variable name\n");
				BUG();
			}
			elem->type = ELEM_TYPE_STRUCT;

			p = fetch_next_str(&q);
			assert(p);

			strncpy(elem->st_name, p, ST_NAME_LEN - 1);
		} else {
			printf("unknown type: %s\n", p);
			BUG();
		}

		p = fetch_next_str(&q);
		assert(p && p[strlen(p) - 1] == ';');

		p[strlen(p) - 1] = 0;

		ret = parse_name_and_dims(elem, p);
		assert(ret == 0);
#if DEBUG
		printf("%s ", elem->name);
#endif
		list_add_tail(&elem->list, &st->elems);
	}
	*str = q;
#if DEBUG
	printf("\n");
#endif

	return 0;
}

struct st *fetch_next_struct(char **str)
{
	char *p = NULL, *q = NULL;
	struct st *st = NULL;
	int ret = 0;

	if (str == NULL || *str == NULL || **str == 0)
		return NULL;

	st = malloc(sizeof(struct st));
	assert(st != NULL);

	memset(st, 0, sizeof(struct st));
	INIT_LIST_HEAD(&st->elems);
	INIT_LIST_HEAD(&st->list);

	q = *str;
	/* find the start for a struct */
	while (*q) {
		p = fetch_next_str(&q);
		if (p == NULL) {
			goto err;
		}
		if (strcmp(p, "struct") == 0)
			break;
	}
	if (!*q) {
		goto err;
	}

	p = fetch_next_str(&q);
	assert(p);
	strncpy(st->name, p, ST_NAME_LEN - 1);

#if DEBUG
	printf("%s\n", st->name);
#endif

	p = fetch_next_str(&q);
	assert(p && strcmp(p, "{") == 0);

	ret = parse_struct(st, &q);
	assert(ret == 0);

	*str = q;

	return st;
err:
	if (st)
		free(st);

	return NULL;
}

int req_resp_filter(char *struct_name)
{
	if (endswith(struct_name, "_req") ||
		endswith(struct_name, "_resp")) {
		printf("%s:%d, strct_name=%s\n", __func__, __LINE__, struct_name);
		return 0;
	}

	return 1;
}

int get_struct_list(char *xdr_file)
{
	FILE *xdr_fp = NULL;
	char *content = NULL;
	char *p = NULL;
	size_t size = 0;
	size_t ret = 0;
	struct st *st = NULL;

	if ((size = file_size(xdr_file)) > SZ_1MB) {
		printf("file %s too long, failed to hanlde\n", xdr_file);
		return -1;
	}
#if DEBUG
	printf("xdr file size: %d\n", size);
#endif

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

#if DEBUG
	printf("begin parse xdr file\n");
#endif
	p = content;
	while (*p) {
		st = fetch_next_struct(&p);
		list_add_tail(&st->list, &st_list);
	}

#if DEBUG
	printf("end parse xdr file\n");
#endif
	free(content);

	return 0;
}

int get_elem_dims(struct elem *elem)
{
	int d = 0;

	for (d = 0; d < DIMS_MAX_NUM && elem->dims[d]; ++d)
		;

	return d;
}

const char* get_type_name(unsigned int type) {
	switch (type) {
		case ELEM_TYPE_CHAR:
			return "char";
		case ELEM_TYPE_INT:
			return "int";
		case ELEM_TYPE_SHORT:
			return "short";
		case ELEM_TYPE_STRUCT:
			return "struct";
		case ELEM_TYPE_UNSIGNED_CHAR:
			return "unsigned char";
		case ELEM_TYPE_UNSIGNED_INT:
			return "unsigned int";
		case ELEM_TYPE_UNSIGNED_SHORT:
			return "unsigned short";
		default:
			return "unknown";
	}
}

void show_elem_list(struct st *st)
{
	struct elem *p = NULL;
	int cnt = 0, d = 0;

	list_for_each_entry(p, &st->elems, list) {
		d = get_elem_dims(p);
		if (d)
			printf("%s %s@%d, ", get_type_name(p->type), p->name, d);
		else
			printf("%s %s, ", get_type_name(p->type), p->name);
		++cnt;
	}
	printf("%d\n", cnt);
}

void show_st_list(void)
{
	struct st *p = NULL;
	int cnt = 0;

	printf("struct name list: \n");
	list_for_each_entry(p, &st_list, list) {
		printf("%s: ", p->name);
		show_elem_list(p);
		++cnt;
	}
	printf("struct count: %d\n", cnt);
}

int gen_h_file(char *h_file)
{
	struct st *p = NULL;
	FILE *fp = NULL;

	fp = fopen(h_file, "w");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open '%s'\n", h_file);
		return -1;
	}

	fprintf(fp, "/*\n * Automatically generated - do not edit\n */\n\n");
	fprintf(fp, "#ifndef _SERIALIZE_RAW_H_\n");
	fprintf(fp, "#define _SERIALIZE_RAW_H_\n\n");

	fprintf(fp, "#include <stdbool.h>\n");
	fprintf(fp, "#include \"common.h\"\n\n");
	list_for_each_entry(p, &st_list, list) {
		fprintf(fp, "bool %s_raw_encode(void *req, unsigned char **buf, int *len,\n", p->name);
		fprintf(fp, "\t\t\tenum free_mode *mode);\n\n");
		fprintf(fp, "void __%s_raw_decode(struct %s *req);\n\n", p->name, p->name);
		fprintf(fp, "bool %s_raw_decode(unsigned char *buf, int len, void **req,\n", p->name);
		fprintf(fp, "\t\t\tenum free_mode *mode);\n\n");
	}
	fprintf(fp, "#endif\n");

	fclose(fp);

	return 0;
}

int get_max_dims_in_elems(struct st *st)
{
	struct elem *p = NULL;
	int max_dims = 0, d = 0;

	list_for_each_entry(p, &st->elems, list) {
		if (p->type == ELEM_TYPE_CHAR || p->type == ELEM_TYPE_UNSIGNED_CHAR)
			continue;
		d = get_elem_dims(p);
		if (d > max_dims) {
			max_dims = d;
		}
	}

	return max_dims;
}

void output_encode(FILE *fp, struct st *st)
{
	int max_dims = get_max_dims_in_elems(st);
	struct elem *p = NULL;
	int d = 0;
	bool write = false;

	fprintf(fp, "bool %s_raw_encode(void *req, unsigned char **buf, int *len,\n", st->name);
	fprintf(fp, "\t\t\tenum free_mode *mode)\n");
	fprintf(fp, "{\n");
	fprintf(fp, "\tDEBUG((DEBUG_DEBUG, \"in\\n\"));\n\n");

	fprintf(fp, "\tbool ret = true;\n");

	switch (max_dims) {
		case 5:
			fprintf(fp, "\tint i = 0, j = 0, k = 0, m = 0, n = 0;\n");
			break;
		case 4:
			fprintf(fp, "\tint i = 0, j = 0, k = 0, m = 0;\n");
			break;
		case 3:
			fprintf(fp, "\tint i = 0, j = 0, k = 0;\n");
			break;
		case 2:
			fprintf(fp, "\tint i = 0, j = 0;\n");
			break;
		case 1:
			fprintf(fp, "\tint i = 0;\n");
			break;
		default:
			break;
	}
	fprintf(fp, "\tstruct %s *_req = req;\n\n", st->name);

	list_for_each_entry(p, &st->elems, list) {
		if (!((p->type & ELEM_TYPE_SHORT) || (p->type & ELEM_TYPE_INT)
					|| (p->type & ELEM_TYPE_STRUCT)))
			continue;
		write = true;
		d = get_elem_dims(p);
		switch (d) {
		case 5:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k)\n", p->dims[2]);
			fprintf(fp, "\t\t\t\tfor (m = 0; m < %s; ++m)\n", p->dims[3]);
			fprintf(fp, "\t\t\t\t\tfor (n = 0; n < %s; ++n) {\n", p->dims[4]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t\t\t_req->%s[i][j][k][m][n] = htons(_req->%s[i][j][k][m][n]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t\t\t_req->%s[i][j][k][m][n] = htonl(_req->%s[i][j][k][m][n]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t\t\t%s_raw_encode(&_req->%s[i][j][k][m][n], NULL, NULL, NULL);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t\t\t}\n");
			break;
		case 4:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k)\n", p->dims[2]);
			fprintf(fp, "\t\t\t\tfor (m = 0; m < %s; ++m) {\n", p->dims[3]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t\t_req->%s[i][j][k][m] = htons(_req->%s[i][j][k][m]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t\t_req->%s[i][j][k][m] = htonl(_req->%s[i][j][k][m]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t\t%s_raw_encode(&_req->%s[i][j][k][m], NULL, NULL, NULL);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t\t}\n");
			break;
		case 3:

			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k) {\n", p->dims[2]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t_req->%s[i][j][k] = htons(_req->%s[i][j][k]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t_req->%s[i][j][k] = htonl(_req->%s[i][j][k]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t%s_raw_encode(&_req->%s[i][j][k], NULL, NULL, NULL);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t}\n");
			break;
		case 2:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j) {\n", p->dims[1]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t_req->%s[i][j] = htons(_req->%s[i][j]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t_req->%s[i][j] = htonl(_req->%s[i][j]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t%s_raw_encode(&_req->%s[i][j], NULL, NULL, NULL);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t}\n");
			break;
		case 1:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i) {\n", p->dims[0]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t_req->%s[i] = htons(_req->%s[i]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t_req->%s[i] = htonl(_req->%s[i]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t%s_raw_encode(&_req->%s[i], NULL, NULL, NULL);\n", p->st_name, p->name);

			} else {
				printf("unknown type: 0x%0x\n", p->type);
				BUG();
			}
			fprintf(fp, "\t}\n");
			break;
		case 0:
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t_req->%s = htons(_req->%s);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t_req->%s = htonl(_req->%s);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t%s_raw_encode(&_req->%s, NULL, NULL, NULL);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			break;
		default:
			BUG();
			break;
		}
	}

	if (write)
		fprintf(fp, "\n");
	fprintf(fp, "\tif (buf && len) {\n");
	fprintf(fp, "\t\t*buf = mp_malloc(g_mp, __func__, sizeof(struct %s));\n",
			st->name);
	fprintf(fp, "\t\tmemcpy(*buf, _req, sizeof(struct %s));\n", st->name);
	fprintf(fp, "\t\t*len = sizeof(struct %s);\n", st->name);
	fprintf(fp, "\t\t*mode = FREE_MODE_MP_FREE;\n");
	fprintf(fp, "\t}\n\n");

	fprintf(fp, "\tDEBUG((DEBUG_DEBUG, \"out\\n\"));\n\n");
	fprintf(fp, "\treturn ret;\n");
	fprintf(fp, "}\n\n");
}

void output_decode(FILE *fp, struct st *st)
{
	int max_dims = get_max_dims_in_elems(st);
	struct elem *p = NULL;
	int d = 0;
	bool write = false;

	fprintf(fp, "bool %s_raw_decode(unsigned char *buf, int len, void **req,\n", st->name);
	fprintf(fp, "\t\t\tenum free_mode *mode)\n");
	fprintf(fp, "{\n");
	fprintf(fp, "\tDEBUG((DEBUG_DEBUG, \"in\\n\"));\n\n");

	fprintf(fp, "\tbool ret = true;\n");

	switch (max_dims) {
		case 5:
			fprintf(fp, "\tint i = 0, j = 0, k = 0, m = 0, n = 0;\n");
			break;
		case 4:
			fprintf(fp, "\tint i = 0, j = 0, k = 0, m = 0;\n");
			break;
		case 3:
			fprintf(fp, "\tint i = 0, j = 0, k = 0;\n");
			break;
		case 2:
			fprintf(fp, "\tint i = 0, j = 0;\n");
			break;
		case 1:
			fprintf(fp, "\tint i = 0;\n");
			break;
		default:
			break;
	}
	fprintf(fp, "\tstruct %s *_req = (struct %s *)mp_malloc(g_mp, __func__,\n"
			"\t\tsizeof(struct %s));\n\n", st->name, st->name, st->name);
	fprintf(fp, "\tmemcpy(_req, buf, len);\n");

	list_for_each_entry(p, &st->elems, list) {
		if (!((p->type & ELEM_TYPE_SHORT) || (p->type & ELEM_TYPE_INT)
					|| (p->type & ELEM_TYPE_STRUCT)))
			continue;
		write = true;
		d = get_elem_dims(p);
		switch (d) {
		case 5:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k)\n", p->dims[2]);
			fprintf(fp, "\t\t\t\tfor (m = 0; m < %s; ++m)\n", p->dims[3]);
			fprintf(fp, "\t\t\t\t\tfor (n = 0; n < %s; ++n) {\n", p->dims[4]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t\t\t_req->%s[i][j][k][m][n] = ntohs(_req->%s[i][j][k][m][n]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t\t\t_req->%s[i][j][k][m][n] = ntohl(_req->%s[i][j][k][m][n]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t\t\t__%s_raw_decode(&_req->%s[i][j][k][m][n]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t\t\t}\n");
			break;
		case 4:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k)\n", p->dims[2]);
			fprintf(fp, "\t\t\t\tfor (m = 0; m < %s; ++m) {\n", p->dims[3]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t\t_req->%s[i][j][k][m] = ntohs(_req->%s[i][j][k][m]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t\t_req->%s[i][j][k][m] = ntohl(_req->%s[i][j][k][m]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t\t__%s_raw_decode(&_req->%s[i][j][k][m]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t\t}\n");
			break;
		case 3:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k) {\n", p->dims[2]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t_req->%s[i][j][k] = ntohs(_req->%s[i][j][k]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t_req->%s[i][j][k] = ntohl(_req->%s[i][j][k]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t__%s_raw_decode(&_req->%s[i][j][k]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t}\n");
			break;
		case 2:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j) {\n", p->dims[1]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t_req->%s[i][j] = ntohs(_req->%s[i][j]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t_req->%s[i][j] = ntohl(_req->%s[i][j]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t__%s_raw_decode(&_req->%s[i][j]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t}\n");
			break;
		case 1:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i) {\n", p->dims[0]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t_req->%s[i] = ntohs(_req->%s[i]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t_req->%s[i] = ntohl(_req->%s[i]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t__%s_raw_decode(&_req->%s[i]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t}\n");
			break;
		case 0:
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t_req->%s = ntohs(_req->%s);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t_req->%s = ntohl(_req->%s);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t__%s_raw_decode(&_req->%s);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			break;
		default:
			BUG();
			break;
		}
	}

	if (write)
		fprintf(fp, "\n");
	fprintf(fp, "\tif (req) {\n");
	fprintf(fp, "\t\t*req = _req;\n");
	fprintf(fp, "\t\t*mode = FREE_MODE_MP_FREE;\n");
	fprintf(fp, "\t}\n\n");

	fprintf(fp, "\tDEBUG((DEBUG_DEBUG, \"out\\n\"));\n\n");
	fprintf(fp, "\treturn ret;\n");
	fprintf(fp, "}\n\n");
}

void output_native_decode(FILE *fp, struct st *st)
{
	int max_dims = get_max_dims_in_elems(st);
	struct elem *p = NULL;
	int d = 0;
	bool write = false;

	bool only_char = true;
	list_for_each_entry(p, &st->elems, list) {
		if (p->type == ELEM_TYPE_INT || p->type == ELEM_TYPE_SHORT ||
			p->type == ELEM_TYPE_STRUCT || p->type == ELEM_TYPE_UNSIGNED_INT ||
			p->type == ELEM_TYPE_UNSIGNED_SHORT) {
			only_char = false;
			break;
		}
	}

	if (only_char)
		fprintf(fp, "void __%s_raw_decode(__attribute__((unused)) struct %s *req)\n", st->name, st->name);
	else
		fprintf(fp, "void __%s_raw_decode(struct %s *req)\n", st->name, st->name);

	fprintf(fp, "{\n");
	fprintf(fp, "\tDEBUG((DEBUG_DEBUG, \"in\\n\"));\n\n");

	switch (max_dims) {
		case 5:
			fprintf(fp, "\tint i = 0, j = 0, k = 0, m = 0, n = 0;\n");
			break;
		case 4:
			fprintf(fp, "\tint i = 0, j = 0, k = 0, m = 0;\n");
			break;
		case 3:
			fprintf(fp, "\tint i = 0, j = 0, k = 0;\n");
			break;
		case 2:
			fprintf(fp, "\tint i = 0, j = 0;\n");
			break;
		case 1:
			fprintf(fp, "\tint i = 0;\n");
			break;
		default:
			break;
	}

	if (!only_char)
		fprintf(fp, "\tstruct %s *_req = req;\n\n", st->name);

	list_for_each_entry(p, &st->elems, list) {
		if (!((p->type & ELEM_TYPE_SHORT) || (p->type & ELEM_TYPE_INT)
					|| (p->type & ELEM_TYPE_STRUCT)))
			continue;
		write = true;
		d = get_elem_dims(p);
		switch (d) {
		case 5:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k)\n", p->dims[2]);
			fprintf(fp, "\t\t\t\tfor (m = 0; m < %s; ++m)\n", p->dims[3]);
			fprintf(fp, "\t\t\t\t\tfor (n = 0; n < %s; ++n) {\n", p->dims[4]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t\t\t_req->%s[i][j][k][m][n] = ntohs(_req->%s[i][j][k][m][n]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t\t\t_req->%s[i][j][k][m][n] = ntohl(_req->%s[i][j][k][m][n]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t\t\t__%s_raw_decode(&_req->%s[i][j][k][m][n]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t\t\t}\n");
			break;
		case 4:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k)\n", p->dims[2]);
			fprintf(fp, "\t\t\t\tfor (m = 0; m < %s; ++m) {\n", p->dims[3]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t\t_req->%s[i][j][k][m] = ntohs(_req->%s[i][j][k][m]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t\t_req->%s[i][j][k][m] = ntohl(_req->%s[i][j][k][m]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t\t__%s_raw_decode(&_req->%s[i][j][k][m]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t\t}\n");
			break;
		case 3:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j)\n", p->dims[1]);
			fprintf(fp, "\t\t\tfor (k = 0; k < %s; ++k) {\n", p->dims[2]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t\t_req->%s[i][j][k] = ntohs(_req->%s[i][j][k]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t\t_req->%s[i][j][k] = ntohl(_req->%s[i][j][k]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t\t__%s_raw_decode(&_req->%s[i][j][k]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t\t}\n");
			break;
		case 2:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i)\n", p->dims[0]);
			fprintf(fp, "\t\tfor (j = 0; j < %s; ++j) {\n", p->dims[1]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t\t_req->%s[i][j] = ntohs(_req->%s[i][j]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t\t_req->%s[i][j] = ntohl(_req->%s[i][j]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t\t__%s_raw_decode(&_req->%s[i][j]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t\t}\n");
			break;
		case 1:
			fprintf(fp, "\tfor (i = 0; i < %s; ++i) {\n", p->dims[0]);
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t\t_req->%s[i] = ntohs(_req->%s[i]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t\t_req->%s[i] = ntohl(_req->%s[i]);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t\t__%s_raw_decode(&_req->%s[i]);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			fprintf(fp, "\t}\n");
			break;
		case 0:
			if (p->type & ELEM_TYPE_SHORT) {
				fprintf(fp, "\t_req->%s = ntohs(_req->%s);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_INT) {
				fprintf(fp, "\t_req->%s = ntohl(_req->%s);\n", p->name, p->name);

			} else if (p->type & ELEM_TYPE_STRUCT) {
				fprintf(fp, "\t__%s_raw_decode(&_req->%s);\n", p->st_name, p->name);

			} else {
				BUG();
			}
			break;
		default:
			BUG();
			break;
		}
	}

	if (write)
		fprintf(fp, "\n");
	fprintf(fp, "\tDEBUG((DEBUG_DEBUG, \"out\\n\"));\n");
	fprintf(fp, "}\n\n");
}

int gen_c_file(char *c_file)
{
	FILE *fp = NULL;
	struct st *p = NULL;

	fp = fopen(c_file, "w");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open '%s'\n", c_file);
		return -1;
	}

	fprintf(fp, "/*\n * Automatically generated - do not edit\n */\n\n");
	fprintf(fp, "#include <arpa/inet.h>\n");
	fprintf(fp, "#include <stdbool.h>\n");
	fprintf(fp, "#include <stdio.h>\n");
	fprintf(fp, "#include <rpc/types.h>\n");
	fprintf(fp, "#include <rpc/xdr.h>\n");
	fprintf(fp, "#include \"common.h\"\n");
	fprintf(fp, "#include \"debug.h\"\n");
	fprintf(fp, "#include \"log.h\"\n");
	fprintf(fp, "#include \"mem_pool.h\"\n");
	fprintf(fp, "#include \"serialize.h\"\n\n");

	list_for_each_entry(p, &st_list, list) {
		output_encode(fp, p);
		output_native_decode(fp, p);
		output_decode(fp, p);
	}

	fclose(fp);

	return 0;
}

int gen_serialize(char *h_file, char *c_file)
{
	if (gen_h_file(h_file) == -1) {
		printf("gen header file failed!\n");
		return -1;
	};

	if (gen_c_file(c_file) == -1) {
		printf("gen src file failed!\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 4) {
		printf("Usage: %s xdr_file h_file c_file\n", argv[0]);
		return -1;
	}

	if (get_struct_list(argv[1]) == -1) {
		printf("get struct list failed\n");
		return -1;
	}

	show_st_list();

	if (gen_serialize(argv[2], argv[3]) == -1) {
		printf("gen serialize failed\n");
		return -1;
	}

	return 0;
}
