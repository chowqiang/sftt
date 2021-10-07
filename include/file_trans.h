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

#ifndef _FILE_TRANS_H_
#define _FILE_TRANS_H_

#include "packet.h"

int send_files_by_get_resp(int fd, char *path, struct sftt_packet *resp_pacekt,
		struct get_resp *resp);

int send_files_by_put_req(int fd, char *path, char *target,
		struct sftt_packet *req_packet, struct put_req *req);

int recv_files_from_get_resp(int fd, char *path, struct sftt_packet *resp_packet);

int recv_files_by_put_req(int fd, struct sftt_packet *req_packet);

#endif
