/*
 * Automatically generated - do not edit
 */

#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include <stdbool.h>

bool validate_req_encode(void *req, unsigned char **buf, int *len);

bool validate_req_decode(unsigned char *buf, int len, void **req);

bool validate_resp_encode(void *req, unsigned char **buf, int *len);

bool validate_resp_decode(unsigned char *buf, int len, void **req);

bool append_conn_req_encode(void *req, unsigned char **buf, int *len);

bool append_conn_req_decode(unsigned char *buf, int len, void **req);

bool append_conn_resp_encode(void *req, unsigned char **buf, int *len);

bool append_conn_resp_decode(unsigned char *buf, int len, void **req);

bool pwd_req_encode(void *req, unsigned char **buf, int *len);

bool pwd_req_decode(unsigned char *buf, int len, void **req);

bool pwd_resp_encode(void *req, unsigned char **buf, int *len);

bool pwd_resp_decode(unsigned char *buf, int len, void **req);

bool ll_req_encode(void *req, unsigned char **buf, int *len);

bool ll_req_decode(unsigned char *buf, int len, void **req);

bool ll_resp_encode(void *req, unsigned char **buf, int *len);

bool ll_resp_decode(unsigned char *buf, int len, void **req);

bool cd_req_encode(void *req, unsigned char **buf, int *len);

bool cd_req_decode(unsigned char *buf, int len, void **req);

bool cd_resp_encode(void *req, unsigned char **buf, int *len);

bool cd_resp_decode(unsigned char *buf, int len, void **req);

bool get_req_encode(void *req, unsigned char **buf, int *len);

bool get_req_decode(unsigned char *buf, int len, void **req);

bool get_resp_encode(void *req, unsigned char **buf, int *len);

bool get_resp_decode(unsigned char *buf, int len, void **req);

bool put_req_encode(void *req, unsigned char **buf, int *len);

bool put_req_decode(unsigned char *buf, int len, void **req);

bool put_resp_encode(void *req, unsigned char **buf, int *len);

bool put_resp_decode(unsigned char *buf, int len, void **req);

bool common_resp_encode(void *req, unsigned char **buf, int *len);

bool common_resp_decode(unsigned char *buf, int len, void **req);

bool write_msg_req_encode(void *req, unsigned char **buf, int *len);

bool write_msg_req_decode(unsigned char *buf, int len, void **req);

bool read_msg_req_encode(void *req, unsigned char **buf, int *len);

bool read_msg_req_decode(unsigned char *buf, int len, void **req);

bool read_msg_resp_encode(void *req, unsigned char **buf, int *len);

bool read_msg_resp_decode(unsigned char *buf, int len, void **req);

bool mp_stat_req_encode(void *req, unsigned char **buf, int *len);

bool mp_stat_req_decode(unsigned char *buf, int len, void **req);

bool mp_stat_resp_encode(void *req, unsigned char **buf, int *len);

bool mp_stat_resp_decode(unsigned char *buf, int len, void **req);

bool directcmd_req_encode(void *req, unsigned char **buf, int *len);

bool directcmd_req_decode(unsigned char *buf, int len, void **req);

bool directcmd_resp_encode(void *req, unsigned char **buf, int *len);

bool directcmd_resp_decode(unsigned char *buf, int len, void **req);

bool who_req_encode(void *req, unsigned char **buf, int *len);

bool who_req_decode(unsigned char *buf, int len, void **req);

bool who_resp_encode(void *req, unsigned char **buf, int *len);

bool who_resp_decode(unsigned char *buf, int len, void **req);

bool write_req_encode(void *req, unsigned char **buf, int *len);

bool write_req_decode(unsigned char *buf, int len, void **req);

bool write_resp_encode(void *req, unsigned char **buf, int *len);

bool write_resp_decode(unsigned char *buf, int len, void **req);

#endif
