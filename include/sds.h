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

#ifndef _SFTT_STRINGS_H_
#define _SFTT_STRINGS_H_

#define MAX_SDS_AUTO_GROW_SIZE	1024

struct sds {
	char *buf;
	int len;
	int size;
	struct pthread_mutex *mutex;
};

struct sds *sds_construct(void);
void sds_destruct(struct sds *ptr);
int sds_add_char(struct sds *ptr, char c);
int sds_add_str(struct sds *ptr, char *str);

#endif
