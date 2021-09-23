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
#include "encrypt.h"

void print_bytes(char *aim);

void print_bytes(char *aim)
{
	int len = strlen(aim);
	int j;
        for (j=0; j < len; j++) {
                printf("%d ",aim[j]);
        }
        printf("\n");
}

unsigned char *sftt_encrypt_func(unsigned char *source , int size)
{

//	print_bytes(source);
	
	int i;
	for(i = 0; i < size; i++){
		source[i] = source[i] + i + 5;
		
	}
	
	return source;

}

unsigned char *sftt_decrypt_func(unsigned char *aim, int size)
{
	int i;
	for(i = 0; i < size; i++ ){
		aim[i] = aim[i] - i - 5;
		
	}

//	print_bytes(aim);

	return aim;
}
