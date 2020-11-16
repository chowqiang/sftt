#ifndef _COMPRESS_H_
#define _COMPRESS_H_

#define CHARSET_SIZE	256
#define CHAR_BIT_LEN	8
#define HUFFMAN_META_DATA_SIZE	(5 * (CHARSET_SIZE + 1))

int huffman_compress(unsigned char *input, int input_len, unsigned char *output);
int huffman_decompress(unsigned char *input, unsigned char *output);


#endif
