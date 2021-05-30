/*
 * Automatically generated - do not edit
 */

#include <stdbool.h>
#include <stdio.h>
#include "log.h"
#include "mem_pool.h"
#include "req_rsp.h"
#include "serialize.h"
#include "xdr.h"

extern struct mem_pool *g_mp;

bool validate_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_validate_req(&xdr, (struct validate_req*)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool validate_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct validate_req *_req = (struct validate_req *)mp_malloc(g_mp, sizeof(struct validate_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_validate_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);
	return ret;
}

bool validate_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_validate_resp(&xdr, (struct validate_resp*)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool validate_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct validate_resp *_req = (struct validate_resp *)mp_malloc(g_mp, sizeof(struct validate_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_validate_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);
	return ret;
}

bool pwd_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_pwd_req(&xdr, (struct pwd_req*)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool pwd_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct pwd_req *_req = (struct pwd_req *)mp_malloc(g_mp, sizeof(struct pwd_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_pwd_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);
	return ret;
}

bool pwd_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_pwd_resp(&xdr, (struct pwd_resp*)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool pwd_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct pwd_resp *_req = (struct pwd_resp *)mp_malloc(g_mp, sizeof(struct pwd_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_pwd_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);
	return ret;
}
