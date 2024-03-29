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

#ifndef _END_POINT_H_
#define _END_POINT_H_

#define UPDATE_THRESHOLD	100
#define SFTT_DEFAULT_PORT	3009

int get_default_port();

int get_pseudo_random_port(void);
int get_real_random_port(void);

int make_client(char *host, int port);

int make_server(int port);
#endif
