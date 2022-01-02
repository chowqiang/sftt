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
#include <string.h>
#include "debug.h"
#include "encrypt.h"

void print_bytes(char *aim);

void print_bytes(char *aim)
{
	DBUG_ENTER(__func__);

	int len = strlen(aim);
	int j;
        for (j=0; j < len; j++) {
                printf("%d ",aim[j]);
        }
        printf("\n");

	DBUG_VOID_RETURN;
}

unsigned char *sftt_encrypt_func(unsigned char *source , int size)
{
	DBUG_ENTER(__func__);

	int i;
	for(i = 0; i < size; i++){
		source[i] = source[i] + i + 5;

	}

	DBUG_RETURN(source);
}

unsigned char *sftt_decrypt_func(unsigned char *aim, int size)
{
	DBUG_ENTER(__func__);

	int i;
	for(i = 0; i < size; i++ ){
		aim[i] = aim[i] - i - 5;

	}

	DBUG_RETURN(aim);
}
