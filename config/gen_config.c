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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct key_value {
	char key[128];
	int key_len;
	char value[1024];
	int value_len;
};

int read_line(FILE *fp, char *buf, int max_len)
{
	int ret = 0;
	char ch;
	int i = 0;

	if (feof(fp))
		return -1;

	while (!feof(fp)) {
		if (fread(&ch, 1, 1, fp) == 0)
			break;

		if (ch == '\n') {
			break;
		}

		buf[i++] = ch;
		if (i >= max_len)
			break;
	}

	buf[i] = 0;

	return 0;
}

int str_to_config(char *buf, struct key_value *config)
{
	char *pos;

	if (buf == NULL || config == NULL)
		return -1;

	if (strlen(buf) == 0)
		return -1;

	pos = index(buf, '=');
	if (pos == NULL) {
		strncpy(config->key, buf, 127);
		config->key_len = strlen(config->key);

		return 0;
	}

	*pos++ = 0;

	strncpy(config->key, buf, 127);
	config->key_len = strlen(config->key);

	strncpy(config->value, pos, 1023);
	config->value_len = strlen(config->value);

	return 0;
}

void write_header(FILE *fp)
{
	fprintf(fp, "/*\n * Automatically generated - do not edit\n */\n\n");
	fprintf(fp, "#ifndef _AUTOCONF_H_\n");
	fprintf(fp, "#define _AUTOCONF_H_\n\n");
}

void write_foot(FILE *fp)
{
	fprintf(fp, "#endif\n");
}

void write_nodef(FILE *fp, char *key)
{
	fprintf(fp, "/* %s is not set */\n", key);
}

void write_def(FILE *fp, char *key)
{
	fprintf(fp, "#define %s 1\n", key);
}

void write_config(FILE *fp, struct key_value *config)
{
	fprintf(fp, "#define %s %s\n", config->key, config->value);
}

int __gen_config(FILE *in_fp, FILE *out_fp)
{
	char buf[1024];
	bool done = false;
	struct key_value config;
	int ret;
	int i = 0;

	if (in_fp == NULL || out_fp == NULL)
		return -1;

	write_header(out_fp);

	while (!done) {
		if (read_line(in_fp, buf, 1023) == -1) {
			if (!feof(in_fp))
				printf("read line from config file failed!\n");
			done = true;
			continue;
		}

		if (strlen(buf) == 0)
			continue;

		memset(&config, 0, sizeof(struct key_value));
		if (str_to_config(buf, &config) == -1) {
			if (config.key_len > 0) {
				write_nodef(out_fp, config.key);
				++i;
			}
			continue;
		}

		if (config.value_len == 1) {
			if (config.value[0] == 'y') {
				write_def(out_fp, config.key);
			} else {
				write_nodef(out_fp, config.key);
			}
			++i;
			continue;
		}

		write_config(out_fp, &config);
		++i;
	}

	write_foot(out_fp);

	if (i == 0)
		return -1;

	return 0;
}

int gen_config(char *in_file, char *out_file)
{
	FILE *in_fp, *out_fp;
	bool ok = false;

	if (in_file == NULL || out_file == NULL) {
		printf("in/out file cannot be nil\n");
		return -1;
	}

	in_fp = fopen(in_file, "r");
	if (in_fp == NULL) {
		printf("cannot open %s\n", in_file);
		goto out;
	}

	out_fp = fopen(out_file, "w");
	if (out_fp == NULL) {
		printf("cannot open %s\n", out_file);
		goto out;
	}

	if (__gen_config(in_fp, out_fp) == -1) {
		printf("cannot gen config!\n");
		goto out;
	}

	ok = true;
out:
	if (in_fp)
		fclose(in_fp);
	if (out_fp)
		fclose(out_fp);

	if (ok == false) {
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s conf autoconf\n", argv[0]);
		return -1;
	}

	if (gen_config(argv[1], argv[2]) == -1) {
		printf("gen config failed!\n");
		return -1;
	}

	return 0;
}
