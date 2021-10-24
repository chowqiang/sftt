/*
 * Automatically generated - do not edit
 */

#ifndef _TRANS_H_
#define _TRANS_H_

#include "packet.h"

int send_validate_resp(int fd, struct sftt_packet *resp_packet,
	struct validate_resp *resp, int code, int next);

int send_append_conn_resp(int fd, struct sftt_packet *resp_packet,
	struct append_conn_resp *resp, int code, int next);

int send_pwd_resp(int fd, struct sftt_packet *resp_packet,
	struct pwd_resp *resp, int code, int next);

int send_ll_resp(int fd, struct sftt_packet *resp_packet,
	struct ll_resp *resp, int code, int next);

int send_cd_resp(int fd, struct sftt_packet *resp_packet,
	struct cd_resp *resp, int code, int next);

int send_get_resp(int fd, struct sftt_packet *resp_packet,
	struct get_resp *resp, int code, int next);

int send_put_resp(int fd, struct sftt_packet *resp_packet,
	struct put_resp *resp, int code, int next);

int send_common_resp(int fd, struct sftt_packet *resp_packet,
	struct common_resp *resp, int code, int next);

int send_read_msg_resp(int fd, struct sftt_packet *resp_packet,
	struct read_msg_resp *resp, int code, int next);

int send_mp_stat_resp(int fd, struct sftt_packet *resp_packet,
	struct mp_stat_resp *resp, int code, int next);

int send_directcmd_resp(int fd, struct sftt_packet *resp_packet,
	struct directcmd_resp *resp, int code, int next);

int send_who_resp(int fd, struct sftt_packet *resp_packet,
	struct who_resp *resp, int code, int next);

int send_write_resp(int fd, struct sftt_packet *resp_packet,
	struct write_resp *resp, int code, int next);

#endif
