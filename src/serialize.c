#include <stdbool.h>
#include <stdio.h>
#include "log.h"
#include "mem_pool.h"
#include "serialize.h"
#include "xdr.h"

extern struct mem_pool *g_mp;

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
	add_log(LOG_INFO, "begin to encode ...");
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_validate_req(&xdr, (validate_req *)req);
	fclose(fp);

	*len = size;
	add_log(LOG_INFO, "end encode|ret: %d, encoded len: %d", ret, *len);

	return ret;
}

bool validate_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "begin to decode ...");
	validate_req *_req = (validate_req *)mp_malloc(g_mp, sizeof(validate_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_validate_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "end decode|ret: %d", ret);

	return ret;
}

bool validate_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "begin to encode ...");
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_validate_resp(&xdr, (validate_resp *)rsp);
	fclose(fp);

	*len = size;
	add_log(LOG_INFO, "end encode|ret: %d, encode len: %d", ret, *len);

	return ret;
}

bool validate_rsp_decode(unsigned char *buf, int len, void **rsp)
{
	add_log(LOG_INFO, "begin to decode ...");
	validate_resp *_rsp = (validate_resp *)mp_malloc(g_mp, sizeof(validate_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_validate_resp(&xdr, (validate_resp *)_rsp);
	fclose(fp);

	*rsp = _rsp;
	add_log(LOG_INFO, "encode decode|ret: %d", ret);

	return ret;
}


bool send_file_name_req_encode(void *req, unsigned char **buf, int *len)
{
	return true;
}

bool send_file_name_req_decode(unsigned char *buf, int len, void **req)
{
	return true;
}

bool send_file_name_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool send_file_name_rsp_decode(unsigned char *buf, int len, void **rsp)
{
	return true;
}


bool send_data_req_encode(void *req, unsigned char **buf, int *len)
{
	return true;
}

bool send_data_req_decode(unsigned char *buf, int len, void **req)
{
	return true;
}

bool send_data_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool send_data_rsp_decode(unsigned char *buf, int len, void **rsp)
{
	return true;
}


bool send_file_end_req_encode(void *req, unsigned char **buf, int *len)
{
	return true;
}

bool send_file_end_req_decode(unsigned char *buf, int len, void **req)
{
	return true;
}

bool send_file_end_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool send_file_end_rsp_decode(unsigned char *buf, int len, void **rsp)
{
	return true;
}


bool send_end_complete_req_encode(void *req, unsigned char **buf, int *len)
{
	return true;
}

bool send_end_complete_req_decode(unsigned char *buf, int len, void **req)
{
	return true;
}

bool send_end_complete_rsp_encode(void *rsp, unsigned char **buf, int *len)
{
	return true;
}

bool send_end_complete_rsp_decode(unsigned char *buf, int len, void **rsp)
{
	return true;
}
