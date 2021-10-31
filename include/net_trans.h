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

#ifndef _NET_TRANS_H_
#define _NET_TRANS_H_

#include "packet.h"

struct sftt_packet *malloc_sftt_packet(void);

int send_sftt_packet(int sock, struct sftt_packet *sp);

int recv_sftt_packet(int sock, struct sftt_packet *sp); 

void free_sftt_packet(struct sftt_packet **sp);

#endif
