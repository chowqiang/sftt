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

#ifndef _SERIALIZE_RAW_H_
#define _SERIALIZE_RWA_H_

#include <stdbool.h>

bool channel_info_req_raw_encode(void *req, unsigned char **buf, int *len);

bool channel_info_req_raw_decode(unsigned char *buf, int len, void **req);

bool channel_info_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool channel_info_resp_raw_decode(unsigned char *buf, int len, void **req);

bool validate_req_raw_encode(void *req, unsigned char **buf, int *len);

bool validate_req_raw_decode(unsigned char *buf, int len, void **req);

bool validate_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool validate_resp_raw_decode(unsigned char *buf, int len, void **req);

bool append_conn_req_raw_encode(void *req, unsigned char **buf, int *len);

bool append_conn_req_raw_decode(unsigned char *buf, int len, void **req);

bool append_conn_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool append_conn_resp_raw_decode(unsigned char *buf, int len, void **req);

bool pwd_req_raw_encode(void *req, unsigned char **buf, int *len);

bool pwd_req_raw_decode(unsigned char *buf, int len, void **req);

bool pwd_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool pwd_resp_raw_decode(unsigned char *buf, int len, void **req);

bool ll_req_raw_encode(void *req, unsigned char **buf, int *len);

bool ll_req_raw_decode(unsigned char *buf, int len, void **req);

bool ll_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool ll_resp_raw_decode(unsigned char *buf, int len, void **req);

bool cd_req_raw_encode(void *req, unsigned char **buf, int *len);

bool cd_req_raw_decode(unsigned char *buf, int len, void **req);

bool cd_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool cd_resp_raw_decode(unsigned char *buf, int len, void **req);

bool get_req_raw_encode(void *req, unsigned char **buf, int *len);

bool get_req_raw_decode(unsigned char *buf, int len, void **req);

bool get_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool get_resp_raw_decode(unsigned char *buf, int len, void **req);

bool put_req_raw_encode(void *req, unsigned char **buf, int *len);

bool put_req_raw_decode(unsigned char *buf, int len, void **req);

bool put_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool put_resp_raw_decode(unsigned char *buf, int len, void **req);

bool common_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool common_resp_raw_decode(unsigned char *buf, int len, void **req);

bool write_msg_req_raw_encode(void *req, unsigned char **buf, int *len);

bool write_msg_req_raw_decode(unsigned char *buf, int len, void **req);

bool read_msg_req_raw_encode(void *req, unsigned char **buf, int *len);

bool read_msg_req_raw_decode(unsigned char *buf, int len, void **req);

bool read_msg_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool read_msg_resp_raw_decode(unsigned char *buf, int len, void **req);

bool mp_stat_req_raw_encode(void *req, unsigned char **buf, int *len);

bool mp_stat_req_raw_decode(unsigned char *buf, int len, void **req);

bool mp_stat_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool mp_stat_resp_raw_decode(unsigned char *buf, int len, void **req);

bool directcmd_req_raw_encode(void *req, unsigned char **buf, int *len);

bool directcmd_req_raw_decode(unsigned char *buf, int len, void **req);

bool directcmd_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool directcmd_resp_raw_decode(unsigned char *buf, int len, void **req);

bool who_req_raw_encode(void *req, unsigned char **buf, int *len);

bool who_req_raw_decode(unsigned char *buf, int len, void **req);

bool who_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool who_resp_raw_decode(unsigned char *buf, int len, void **req);

bool write_req_raw_encode(void *req, unsigned char **buf, int *len);

bool write_req_raw_decode(unsigned char *buf, int len, void **req);

bool write_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool write_resp_raw_decode(unsigned char *buf, int len, void **req);

bool port_update_req_raw_encode(void *req, unsigned char **buf, int *len);

bool port_update_req_raw_decode(unsigned char *buf, int len, void **req);

bool port_update_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool port_update_resp_raw_decode(unsigned char *buf, int len, void **req);

bool reconnect_req_raw_encode(void *req, unsigned char **buf, int *len);

bool reconnect_req_raw_decode(unsigned char *buf, int len, void **req);

bool reconnect_resp_raw_encode(void *req, unsigned char **buf, int *len);

bool reconnect_resp_raw_decode(unsigned char *buf, int len, void **req);

#endif
