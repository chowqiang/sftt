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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "compress.h"
#include "encrypt.h"
#include "encode.h"
#include "mem_pool.h"

extern struct mem_pool *g_mp;

/*
 * Encode a sftt packet.
 */
int sftt_buffer_encode(unsigned char *input, int len, unsigned char **output,
		bool zip, bool crypt)
{
	int output_len = len;

	if (zip) {
		output_len = huffman_compress(input, len, output);

		if (crypt) {
			sftt_encrypt_func(*output, output_len);
		}

		return output_len;

	} else if (crypt) {
		*output = mp_malloc(g_mp, __func__, output_len * sizeof(unsigned char));

		sftt_encrypt_func(*output, output_len);

		return output_len;
	} else {
		*output = mp_malloc(g_mp, __func__, output_len * sizeof(unsigned char));
		memcpy(*output, input, output_len);

		return output_len;
	}
}

/*
 * Decode a sftt packet.
 */
int sftt_buffer_decode(unsigned char *input, int len, unsigned char **output,
		bool unzip, bool decrypt)
{
	int output_len = len;
	unsigned char *tmp;

	if (decrypt) {
		tmp = mp_malloc(g_mp, __func__, len * sizeof(unsigned char));
		memcpy(tmp, input, len);
		sftt_decrypt_func(tmp, len);

		if (unzip) {
			output_len = huffman_decompress(tmp, output);
		} else {
			*output = mp_malloc(g_mp, "output_bufffer", output_len * sizeof(unsigned char));
			memcpy(*output, tmp, output_len);
		}

		mp_free(g_mp, tmp);

		return output_len;

	} else if (unzip) {
		output_len = huffman_decompress(input, output);

		return output_len;
	} else {
		*output = mp_malloc(g_mp, __func__, output_len * sizeof(unsigned char));
		memcpy(*output, input, output_len);

		return output_len;
	}
}
