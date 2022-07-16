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
#include "compress.h"
#include "file.h"

void test_basic(void)
{
	int i = 0;
	char *text = "aaaabbbccd";
	int text_len = strlen(text);
	unsigned char *output;
#if 0
	unsigned char *output = (unsigned char *)mp_malloc(g_mp, __func__,
			sizeof(unsigned char) * 1024);
#endif
	int out_len = huffman_compress((unsigned char *)text, text_len, &output);

	for (i = 0; i < out_len; ++i) {
		printf("%0x", output[i]);
	}
	printf("\n");
}

void test_file(void)
{
	size_t file_size = 0;

	unsigned char *contents = file_get_contents("./dlist.c", &file_size);
	if (contents == NULL) {
		printf("get file contents failed!\n");
		return ;
	}

#if 0
	unsigned char *encodes = (unsigned char *)mp_malloc(g_mp,
			"test_file_encode",CHARSET_SIZE * sizeof(int) + 5 * file_size);
	if (encodes == NULL) {
		printf("alloc merroy for encode failed!\n");
		return ;
	}
#endif
	unsigned char *encodes;

	int encode_len = huffman_compress(contents, file_size, &encodes);
	printf("compress: file_size: %ld, encode_len: %d\n", file_size, encode_len);

#if 0
	unsigned char *decodes = (unsigned char *)mp_malloc(g_mp,
			"test_file_decode", file_size + 1);
	if (decodes == NULL) {
		printf("alloc merroy for decode failed!\n");
		return ;
	}
#endif
	unsigned char *decodes;
	
	int decode_len = huffman_decompress(encodes, &decodes);
	printf("decompress: encode_len: %d, decode_len: %d\n", encode_len, decode_len);

	file_put_contents("./tmp.txt", decodes, (size_t)decode_len);

	return ;	
}

int main(void)
{
	test_basic();

	return 0;
}
