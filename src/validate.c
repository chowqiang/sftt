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

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "validate.h"

char *is_valid_dotted_decimal(char *p, int is_first, int is_last, int *pvalid)
{
	//printf("input dotted decimal: %s\n", p);
	if (!is_first) {
		if (p[0] != '.') {
			printf("first char for dotted decimal should be '.'!\n");
			goto RET_INVALID;
		}
		p++;
	}

	char tmp[4] = {0, 0, 0, 0};
	int j = 0;
	while (p[j] && p[j] != '.' && j < 3) {
		tmp[j] = p[j]; 
		j++;
	}
	//printf("parsed dotted decimal block: %s\n", tmp);

	if (j == 0) {
		printf("dotted decimal length should not be zero!\n");
		goto RET_INVALID;
	}

	if (is_last && p[j]) {
		printf("last dotted decimal length should be less 4!\n");
		goto RET_INVALID; 
	}

	int decimal = atoi(tmp);
	if (tmp[0] == '0' && decimal != 0) {
		printf("dotted decimal should not have redundant zero!\n");
		goto RET_INVALID;
	}

	if (!(decimal >= 0 && decimal <= 255)) {
		printf("dotted decimal should between 0 and 255!\n");
		goto RET_INVALID;
	}

	*pvalid = 1;

	return p + j;

RET_INVALID:
	*pvalid = 0;

	return NULL;
}
 
int is_valid_ipv4(char *ip)
{
	int len = strlen(ip);
	if (len < 7 || len > 15) {
		printf("invalid length for ipv4!\n");
		return 0;
	}
	int i = 0, j = 0;
	for (i = 0; i < len; ++i) {
		if (!((ip[i] >= '0' && ip[i] <= '9') || ip[i] == '.')) {
			printf("invalid char for ipv4!\n");
			return 0;
		}
	}

	int is_valid = 0;
	char *p = ip;
	for (i = 0; i < 4; ++i) {
		p = is_valid_dotted_decimal(p, (i == 0), (i == 3), &is_valid);
		if (!is_valid) {
			printf("invalid dotted decimal for ipv4 in %d-th block!\n", (i + 1));
			return 0;
		}
	}

	return 1;
}
