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

#include <stdio.h>
#include "strtox.h"

void test_parse_args(void)
{
	char *str[] = {"freq=1300", "memsize1=60", "memsize2=180"};
	int freq = 0;
	int memsize1 = 0;
	int memsize2 = 0;

	char **p = str;
	parse_even_earlier((u32 *)&freq, "freq", *p);
	++p;
	parse_even_earlier((u32 *)&memsize1, "memsize1", *p);
	++p;
	parse_even_earlier((u32 *)&memsize2, "memsize2", *p);
	++p;

	printf("freq: %d, memsize1: %d, memsize2: %d\n", freq, memsize1, memsize2);
}

void test_strncpy(void)
{
	char tmp[16];

	memset(tmp, 1, sizeof(tmp));
	strncpy(tmp, "123.123.123.123", 16);

	printf("%s\n", tmp);
}

int main(void)
{
	test_strncpy();

	return 0;
}
