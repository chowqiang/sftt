#ifndef _BITS_H_
#define _BITS_H_

void set_bit(unsigned char *ch, int pos, unsigned char bit);
unsigned char *set_bits(unsigned char *dst, int pos, unsigned int src, int len, int *end); 

#endif
