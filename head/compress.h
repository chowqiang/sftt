#ifndef _COMPRESS_H_
#define _COMPRESS_H_

int huffman_compress(unsigned char *input, int input_len, unsigned char **output);
int huffman_decompress(unsigned char *input, int input_len, unsigned char **output);


#endif
