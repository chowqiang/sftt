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
#include <stdio.h>
#include <string.h>
#include "bits.h"

void set_bit(unsigned char *ch, int pos, unsigned char bit)
{
	unsigned char mask = 0;
	if (bit == 0) {
		mask = 0xff;
		mask = (mask << pos) | (mask >> (8 - pos + 1));
		*ch = *ch & mask;
	} else {
		mask = 1;	
		*ch = *ch | (mask << (pos - 1));
	}	
}

unsigned char get_bit(unsigned char ch, int pos)
{
	unsigned char mask = 1;
	if (ch & (mask << (pos - 1))) {
		return 1;
	}
	
	return 0;
}

unsigned char *set_bits(unsigned char *dst, int pos, unsigned int src,
	int len, int *end)
{
	return NULL;
}

void test_code(void)
{
	char *ac = "1";
	char *bc = "01";
	char *cc = "000";
	char *dc = "001";
	char *p = NULL;
	char *text = "aaaabbbccd";
	unsigned char codes[128];
	unsigned char *pos = codes, *q = NULL;
	
	memset(codes, 0, 128);	

	int i = 0, j = 0, index = 8;
	for (i = 0; i < text[i]; ++i) {
		switch (text[i]) {
		case 'a':
			p = ac;
			break;
		case 'b':
			p = bc;
			break;
		case 'c':
			p = cc;
			break;
		case 'd':
			p = dc;
			break;
		}
		for (j = 0; p[j]; ++j) {
			if (index == 0) {
				index = 8;
				++pos;		
			}
			set_bit(pos, index--, p[j] == '0' ? 0 : 1);
		}
	}
	q = codes;
	while (q <= pos) {
		printf("%0x", *q);
		++q;
	}
	printf("\n");
}

void test_bit_layout(void)
{
	unsigned char c = 1;
	int i = 0;
	for (i = 0; i < 8; ++i) {
		if ((1 << i) & c) {
			printf("%d\n", i);
		}
	}
}

void test_set_bit(void)
{
	unsigned char a[2];
		
	memset(a, 0, sizeof(a));
	set_bit(a, 8, 1);
	set_bit(a, 7, 1);
	set_bit(a, 6, 0);
	set_bit(a, 5, 0);
	set_bit(a, 4, 0);
	set_bit(a, 3, 0);
	set_bit(a, 2, 0);
	set_bit(a, 1, 0);
	set_bit(a + 1, 8, 0);
	set_bit(a + 1, 7, 0);
	set_bit(a + 1, 6, 1);
	set_bit(a + 1, 5, 1);
	set_bit(a + 1, 4, 0);
	set_bit(a + 1, 3, 0);
	set_bit(a + 1, 2, 0);
	set_bit(a + 1, 1, 0);
	printf("0x%x0x%x\n", a[0], a[1]);
}
