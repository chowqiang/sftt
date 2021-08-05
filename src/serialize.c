/*
 * Automatically generated - do not edit
 */

#include <stdbool.h>
#include <stdio.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include "log.h"
#include "mem_pool.h"
#include "req_rsp.h"
#include "serialize.h"

extern struct mem_pool *g_mp;

bool validate_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_validate_req(&xdr, (struct validate_req *)req);

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

	int ret = xdr_validate_resp(&xdr, (struct validate_resp *)req);

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

	int ret = xdr_pwd_req(&xdr, (struct pwd_req *)req);

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

	int ret = xdr_pwd_resp(&xdr, (struct pwd_resp *)req);

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

bool ll_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_ll_req(&xdr, (struct ll_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool ll_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct ll_req *_req = (struct ll_req *)mp_malloc(g_mp, sizeof(struct ll_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_ll_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool ll_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_ll_resp(&xdr, (struct ll_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool ll_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct ll_resp *_req = (struct ll_resp *)mp_malloc(g_mp, sizeof(struct ll_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_ll_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool cd_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_cd_req(&xdr, (struct cd_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool cd_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct cd_req *_req = (struct cd_req *)mp_malloc(g_mp, sizeof(struct cd_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_cd_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool cd_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_cd_resp(&xdr, (struct cd_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool cd_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct cd_resp *_req = (struct cd_resp *)mp_malloc(g_mp, sizeof(struct cd_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_cd_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool get_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_get_req(&xdr, (struct get_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool get_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct get_req *_req = (struct get_req *)mp_malloc(g_mp, sizeof(struct get_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_get_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool get_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_get_resp(&xdr, (struct get_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool get_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct get_resp *_req = (struct get_resp *)mp_malloc(g_mp, sizeof(struct get_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_get_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool put_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_put_req(&xdr, (struct put_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool put_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct put_req *_req = (struct put_req *)mp_malloc(g_mp, sizeof(struct put_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_put_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool put_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_put_resp(&xdr, (struct put_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool put_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct put_resp *_req = (struct put_resp *)mp_malloc(g_mp, sizeof(struct put_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_put_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool common_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_common_resp(&xdr, (struct common_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool common_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct common_resp *_req = (struct common_resp *)mp_malloc(g_mp, sizeof(struct common_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_common_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool write_msg_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_write_msg_req(&xdr, (struct write_msg_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool write_msg_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct write_msg_req *_req = (struct write_msg_req *)mp_malloc(g_mp, sizeof(struct write_msg_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_write_msg_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool read_msg_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_read_msg_req(&xdr, (struct read_msg_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool read_msg_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct read_msg_req *_req = (struct read_msg_req *)mp_malloc(g_mp, sizeof(struct read_msg_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_read_msg_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool read_msg_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_read_msg_resp(&xdr, (struct read_msg_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d", __func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool read_msg_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct read_msg_resp *_req = (struct read_msg_resp *)mp_malloc(g_mp, sizeof(struct read_msg_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_read_msg_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}
