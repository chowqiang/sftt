/*
 * Copyright (C) copy from linux kernel.
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

#ifndef _KSTRTOX_H_
#define _KSTRTOX_H_

#include <stdint.h>
#include <string.h>

#define KSTRTOX_OVERFLOW    (1U << 31)

typedef unsigned int u32;

int kstrtou32(const char *s, unsigned int base, u32 *res);

int kstrtouint(const char *s, unsigned int base, unsigned int *res);

int kstrtoull(const char *s, unsigned int base, unsigned long long *res);

//static int _kstrtoull(const char *s, unsigned int base, unsigned long long *res);

const char *_parse_integer_fixup_radix(const char *s, unsigned int *base);

//static inline char _tolower(const char c);

//static inline int isdigit(int ch);

//static inline int isxdigit(int ch);

unsigned int _parse_integer(const char *s, unsigned int base, unsigned long long *p);

int parse_even_earlier(u32 *res, const char *option, char *p);

#endif
