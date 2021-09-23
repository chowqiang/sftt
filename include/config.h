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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string.h>
#include "req_resp.h"

#define BUFFER_SIZE	10240

#define CONFIG_LINE_MAX_SIZE	1024
#define CONFIG_NAME_MAX_LEN 	256
#define CONFIG_VALUE_MAX_LEN	512

/*
 * host info
 */
#define HOST_MAX_LEN		32


struct sftt_server_config {
	char store_path[FILE_NAME_MAX_LEN];
	char log_dir[DIR_PATH_MAX_LEN];
	int block_size;
	int update_th;
};

struct sftt_client_config {
	int block_size;
	char log_dir[DIR_PATH_MAX_LEN];
};

/*
 * get sftt server config
 */
int get_sftt_server_config(struct sftt_server_config *ssc);

int get_sftt_client_config(struct sftt_client_config *scc);

#endif
