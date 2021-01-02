#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "compress.h"
#include "encrypt.h"
#include "encode.h"

int sftt_buffer_encode(unsigned char *input, int len, unsigned char *output, bool zip, bool crypt) {
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

int sftt_buffer_decode(unsigned char *input, int len, unsigned char *output, bool unzip, bool decrypt) {
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
