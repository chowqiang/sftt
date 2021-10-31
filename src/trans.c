/*
 * Automatically generated - do not edit
 */

#include "net_trans.h"
#include "req_resp.h"
#include "response.h"
#include "trans.h"

extern const char *resp_messages[];

int send_validate_resp(int fd, struct sftt_packet *resp_packet,
	struct validate_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_VALIDATE_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_append_conn_resp(int fd, struct sftt_packet *resp_packet,
	struct append_conn_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_APPEND_CONN_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_pwd_resp(int fd, struct sftt_packet *resp_packet,
	struct pwd_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_PWD_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_ll_resp(int fd, struct sftt_packet *resp_packet,
	struct ll_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_LL_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_cd_resp(int fd, struct sftt_packet *resp_packet,
	struct cd_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_CD_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_get_resp(int fd, struct sftt_packet *resp_packet,
	struct get_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_GET_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_put_resp(int fd, struct sftt_packet *resp_packet,
	struct put_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_PUT_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_common_resp(int fd, struct sftt_packet *resp_packet,
	struct common_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_COMMON_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_read_msg_resp(int fd, struct sftt_packet *resp_packet,
	struct read_msg_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_READ_MSG_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_mp_stat_resp(int fd, struct sftt_packet *resp_packet,
	struct mp_stat_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_MP_STAT_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_directcmd_resp(int fd, struct sftt_packet *resp_packet,
	struct directcmd_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_DIRECTCMD_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_who_resp(int fd, struct sftt_packet *resp_packet,
	struct who_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_WHO_RESP;
	return send_sftt_packet(fd, resp_packet);
}

int send_write_resp(int fd, struct sftt_packet *resp_packet,
	struct write_resp *resp, int code, int next)
{
	resp->status = code;
	strncpy(resp->message, resp_messages[code], RESP_MESSAGE_MAX_LEN - 1);
	resp->next = next;

	resp_packet->obj = resp;
	resp_packet->type = PACKET_TYPE_WRITE_RESP;
	return send_sftt_packet(fd, resp_packet);
}
