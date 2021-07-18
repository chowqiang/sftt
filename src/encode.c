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

/*
 * Encode a sftt packet.
 */
int sftt_buffer_encode(unsigned char *input, int len, unsigned char *output,
		bool zip, bool crypt)
{
	if (!zip && !crypt) {
		memcpy(output, input, len);
		return len;
	}

	int zip_len = len;
	if (zip) {
		zip_len = huffman_compress(input, len, output);
	}

	if (crypt) {
		sftt_encrypt_func(output, zip_len);
	}

	return zip_len;

}

/*
 * Decode a sftt packet.
 */
int sftt_buffer_decode(unsigned char *input, int len, unsigned char *output,
		bool unzip, bool decrypt)
{
	if (!unzip && !decrypt) {
		memcpy(output, input, len);
		return len;
	}

	if (decrypt) {
		sftt_decrypt_func(input, len);
	}

	int unzip_len = len;
	if (unzip) {
		unzip_len = huffman_decompress(input, output);
	}

	return unzip_len;
}
