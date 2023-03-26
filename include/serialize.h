/*
 * Automatically generated - do not edit
 */

#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include <stdbool.h>
#include "common.h"

bool channel_info_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool channel_info_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool channel_info_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool channel_info_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool validate_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool validate_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool validate_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool validate_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool append_conn_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool append_conn_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool append_conn_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool append_conn_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool pwd_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool pwd_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool pwd_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool pwd_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool ll_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool ll_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool ll_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool ll_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool cd_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool cd_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool cd_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool cd_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool get_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool get_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool get_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool get_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool put_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool put_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool put_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool put_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool common_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool common_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool write_msg_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool write_msg_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool read_msg_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool read_msg_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool read_msg_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool read_msg_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool mp_stat_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool mp_stat_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool mp_stat_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool mp_stat_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool directcmd_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool directcmd_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool directcmd_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool directcmd_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool who_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool who_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool who_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool who_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool write_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool write_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool write_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool write_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool port_update_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool port_update_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool port_update_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool port_update_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool reconnect_req_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool reconnect_req_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool reconnect_resp_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

bool reconnect_resp_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

#endif
