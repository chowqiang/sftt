/*
 * Automatically generated - do not edit
 */

#ifndef _SERIALIZE_RAW_H_
#define _SERIALIZE_RAW_H_

#include <stdbool.h>
#include "common.h"

bool version_info_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __version_info_raw_decode(struct version_info *req);

bool version_info_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool channel_info_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __channel_info_req_raw_decode(struct channel_info_req *req);

bool channel_info_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool channel_info_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __channel_info_resp_data_raw_decode(struct channel_info_resp_data *req);

bool channel_info_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool channel_info_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __channel_info_resp_raw_decode(struct channel_info_resp *req);

bool channel_info_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool validate_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __validate_req_raw_decode(struct validate_req *req);

bool validate_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool validate_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __validate_resp_data_raw_decode(struct validate_resp_data *req);

bool validate_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool validate_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __validate_resp_raw_decode(struct validate_resp *req);

bool validate_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool append_conn_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __append_conn_req_raw_decode(struct append_conn_req *req);

bool append_conn_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool append_conn_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __append_conn_resp_data_raw_decode(struct append_conn_resp_data *req);

bool append_conn_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool append_conn_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __append_conn_resp_raw_decode(struct append_conn_resp *req);

bool append_conn_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool logged_in_user_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __logged_in_user_raw_decode(struct logged_in_user *req);

bool logged_in_user_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool pwd_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __pwd_req_raw_decode(struct pwd_req *req);

bool pwd_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool pwd_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __pwd_resp_data_raw_decode(struct pwd_resp_data *req);

bool pwd_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool pwd_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __pwd_resp_raw_decode(struct pwd_resp *req);

bool pwd_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool ll_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __ll_req_raw_decode(struct ll_req *req);

bool ll_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool file_entry_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __file_entry_raw_decode(struct file_entry *req);

bool file_entry_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool ll_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __ll_resp_data_raw_decode(struct ll_resp_data *req);

bool ll_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool ll_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __ll_resp_raw_decode(struct ll_resp *req);

bool ll_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool cd_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __cd_req_raw_decode(struct cd_req *req);

bool cd_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool cd_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __cd_resp_data_raw_decode(struct cd_resp_data *req);

bool cd_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool cd_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __cd_resp_raw_decode(struct cd_resp *req);

bool cd_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool get_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __get_req_raw_decode(struct get_req *req);

bool get_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool trans_entry_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __trans_entry_raw_decode(struct trans_entry *req);

bool trans_entry_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool get_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __get_resp_data_raw_decode(struct get_resp_data *req);

bool get_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool get_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __get_resp_raw_decode(struct get_resp *req);

bool get_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool put_req_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __put_req_data_raw_decode(struct put_req_data *req);

bool put_req_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool put_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __put_req_raw_decode(struct put_req *req);

bool put_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool put_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __put_resp_raw_decode(struct put_resp *req);

bool put_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool common_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __common_resp_raw_decode(struct common_resp *req);

bool common_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool write_msg_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __write_msg_req_raw_decode(struct write_msg_req *req);

bool write_msg_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool read_msg_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __read_msg_req_raw_decode(struct read_msg_req *req);

bool read_msg_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool read_msg_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __read_msg_resp_data_raw_decode(struct read_msg_resp_data *req);

bool read_msg_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool read_msg_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __read_msg_resp_raw_decode(struct read_msg_resp *req);

bool read_msg_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool mp_stat_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __mp_stat_req_raw_decode(struct mp_stat_req *req);

bool mp_stat_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool mp_stat_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __mp_stat_resp_data_raw_decode(struct mp_stat_resp_data *req);

bool mp_stat_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool mp_stat_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __mp_stat_resp_raw_decode(struct mp_stat_resp *req);

bool mp_stat_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool directcmd_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __directcmd_req_raw_decode(struct directcmd_req *req);

bool directcmd_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool directcmd_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __directcmd_resp_data_raw_decode(struct directcmd_resp_data *req);

bool directcmd_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool directcmd_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __directcmd_resp_raw_decode(struct directcmd_resp *req);

bool directcmd_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool who_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __who_req_raw_decode(struct who_req *req);

bool who_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool who_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __who_resp_data_raw_decode(struct who_resp_data *req);

bool who_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool who_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __who_resp_raw_decode(struct who_resp *req);

bool who_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool write_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __write_req_raw_decode(struct write_req *req);

bool write_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool write_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __write_resp_data_raw_decode(struct write_resp_data *req);

bool write_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool write_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __write_resp_raw_decode(struct write_resp *req);

bool write_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool port_update_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __port_update_req_raw_decode(struct port_update_req *req);

bool port_update_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool port_update_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __port_update_resp_raw_decode(struct port_update_resp *req);

bool port_update_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool reconnect_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __reconnect_req_raw_decode(struct reconnect_req *req);

bool reconnect_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

bool reconnect_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode);

void __reconnect_resp_raw_decode(struct reconnect_resp *req);

bool reconnect_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode);

#endif
