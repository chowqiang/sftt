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

int main(void) {
	char *str[] = {"freq=1300", "memsize1=60", "memsize2=180"};
	int freq = 0;
	int memsize1 = 0;
	int memsize2 = 0;

	char **p = str;
	parse_even_earlier(&freq, "freq", *p);
	++p;
	parse_even_earlier(&memsize1, "memsize1", *p);
	++p;
	parse_even_earlier(&memsize2, "memsize2", *p);
	++p;

	printf("freq: %d, memsize1: %d, memsize2: %d\n", freq, memsize1, memsize2);

	return 0;
}
