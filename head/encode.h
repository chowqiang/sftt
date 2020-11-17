#ifndef _ENCODE_H_
#define _ENCODE_H_

int sftt_buffer_encode(unsigned char *input, int len, unsigned char *output, bool zip, bool crypt);
int sftt_buffer_decode(unsigned char *input, int len, unsigned char *output, bool unzip, bool decrypt);

#endif
