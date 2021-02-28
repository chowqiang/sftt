#include <rpc/xdr.h>
#include <stdbool.h>
#include "mem_pool.h"
#include "packet.h"
#include "serialize.h"

struct serialize_handle serializables[] = {
	{PACKET_TYPE_VALIDATE_REQ, validate_req_encode, validate_req_decode},
	{PACKET_TYPE_VALIDATE_RSP, validate_rsp_encode, validate_rsp_decode},
	{PACKET_TYPE_FILE_NAME_REQ, send_file_name_req_encode, send_file_name_req_decode},
	{PACKET_TYPE_FILE_NAME_RSP, send_file_name_rsp_encode, send_file_name_rsp_decode},
	{PACKET_TYPE_DATA_REQ, send_data_req_encode, send_data_req_decode},
	{PACKET_TYPE_DATA_RSP, send_data_rsp_encode, send_data_rsp_decode},
	{PACKET_TYPE_FILE_END_REQ, send_data_req_encode, send_data_req_decode},
	{PACKET_TYPE_FILE_END_RSP, send_data_rsp_encode, send_data_rsp_decode},
	{PACKET_TYPE_SEND_COMPLETE_REQ, send_end_complete_req_encode, send_end_complete_req_decode},
	{PACKET_TYPE_SEND_COMPLETE_RSP, send_end_complete_rsp_encode, send_end_complete_rsp_decode},
	{-1, NULL, NULL},
};

bool validate_req_encode(void *req, unsigned char **buf, int *len)
{
	mem_pool *mp = get_singleton_mp();

	*len = 2 * sizeof(validate_req);
	*buf = (unsigned char *)mp_malloc(mp, sizeof(unsigned char) * (*len));

	XDR xdr;
	validate_req *vreq = (validate_req *)req;

	xdrmem_create(&xdr, *buf, *len, XDR_ENCODE);

	bool status = xdr_int(&xdr, &(vreq->name_len)) &&
				xdr_int(&xdr, &(vreq->passwd_len)) &&
				xdr_string(&xdr, &(vreq->name), USER_NAME_MAX_LEN) &&
				xdr_string(&xdr, &(vreq->passwd_md5), MD5_LEN + 1);

	return status;
}

bool validate_req_decode(unsigned char *buf, int len, void *req)
{
	XDR xdr;
	validate_req *vreq = (validate_req *)req;

	xdrmem_create(&xdr, buf, len, XDR_DECODE);

	bool status = xdr_int(&xdr, &(vreq->name_len)) &&
				xdr_int(&xdr, &(vreq->passwd_len)) &&
				xdr_string(&xdr, &(vreq->name), USER_NAME_MAX_LEN) &&
				xdr_string(&xdr, &(vreq->passwd_md5), MD5_LEN + 1);

	return status;
}

bool validate_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool validate_rsp_decode(unsigned char *buf, int len, void *rsp)
{
	return true;
}


bool send_file_name_req_encode(void *req, unsigned char **buf, int *len)
{
	return true;
}

bool send_file_name_req_decode(unsigned char *buf, int len, void *req)
{
	return true;
}

bool send_file_name_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool send_file_name_rsp_decode(unsigned char *buf, int len, void *rsp)
{
	return true;
}


bool send_data_req_encode(void *req, unsigned char **buf, int *len)
{
	return true;
}

bool send_data_req_decode(unsigned char *buf, int len, void *req)
{
	return true;
}

bool send_data_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool send_data_rsp_decode(unsigned char *buf, int len, void *rsp)
{
	return true;
}


bool send_file_end_req_encode(void *req, unsigned char **buf, int *len)
{
	return true;
}

bool send_file_end_req_decode(unsigned char *buf, int len, void *req)
{
	return true;
}

bool send_file_end_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool send_file_end_rsp_decode(unsigned char *buf, int len, void *rsp)
{
	return true;
}


bool send_end_complete_req_encode(void *req, unsigned char **buf, int *len)
{
	return true;
}

bool send_end_complete_req_decode(unsigned char *buf, int len, void *req)
{
	return true;
}

bool send_end_complete_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool send_end_complete_rsp_decode(unsigned char *buf, int len, void *rsp)
{
	return true;
}
