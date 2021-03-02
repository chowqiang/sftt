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

	XDR xdr;
	validate_req *vreq = (validate_req *)req;

	char *name = (unsigned char *)mp_malloc(mp, sizeof(unsigned char) * (USER_NAME_MAX_LEN + 1));
	char *passwd_md5 = (unsigned char *)mp_malloc(mp, sizeof(unsigned char) * (MD5_LEN + 1));

	strcpy(name, vreq->name);
	strcpy(passwd_md5, vreq->passwd_md5);

	*len = 2 * sizeof(validate_req);
	*buf = (unsigned char *)mp_malloc(mp, sizeof(unsigned char) * (*len));

	xdrmem_create(&xdr, *buf, *len, XDR_ENCODE);

	bool status = xdr_int(&xdr, &(vreq->name_len)) &&
				xdr_int(&xdr, &(vreq->passwd_len)) &&
				xdr_string(&xdr, &name, USER_NAME_MAX_LEN) &&
				xdr_string(&xdr, &passwd_md5, MD5_LEN + 1);

	mp_free(mp, name);
	mp_free(mp, passwd_md5);

	return status;
}

bool validate_req_decode(unsigned char *buf, int len, void *req)
{
	mem_pool *mp = get_singleton_mp();

	XDR xdr;
	validate_req *vreq = (validate_req *)req;

	char *name = (char *)mp_malloc(mp, sizeof(char) * (USER_NAME_MAX_LEN + 1));
	char *passwd_md5 = (char *)mp_malloc(mp, sizeof(char) * (MD5_LEN + 1));

	xdrmem_create(&xdr, buf, len, XDR_DECODE);

	bool status = xdr_int(&xdr, &(vreq->name_len)) &&
				xdr_int(&xdr, &(vreq->passwd_len)) &&
				xdr_string(&xdr, &name, USER_NAME_MAX_LEN) &&
				xdr_string(&xdr, &passwd_md5, MD5_LEN + 1);

	if (status) {
		strcpy(vreq->name, name);
		strcpy(vreq->passwd_md5, passwd_md5);
	}

	mp_free(mp, name);
	mp_free(mp, passwd_md5);

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
