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

#ifndef _BITS_H_
#define _BITS_H_

#define UINT32_C(c)    c ## U
#define BIT32(nr) (UINT32_C(1) << (nr))

#define TYPE_INT_BITS	(sizeof(int) * 8)

/**
 *
 **/
void set_bit(unsigned char *ch, int pos, unsigned char bit);

/**
 *
 **/
unsigned char get_bit(unsigned char ch, int pos);

/**
 *
 **/
unsigned char *set_bits(unsigned char *dst, int pos, unsigned int src, int len, int *end);

#endif
