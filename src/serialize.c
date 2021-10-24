/*
 * Automatically generated - do not edit
 */

#include <stdbool.h>
#include <stdio.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include "log.h"
#include "mem_pool.h"
#include "req_resp.h"
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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool validate_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct validate_req *_req = (struct validate_req *)mp_malloc(g_mp,
		__func__, sizeof(struct validate_req));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool validate_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct validate_resp *_req = (struct validate_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct validate_resp));

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

bool append_conn_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_append_conn_req(&xdr, (struct append_conn_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool append_conn_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct append_conn_req *_req = (struct append_conn_req *)mp_malloc(g_mp,
		__func__, sizeof(struct append_conn_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_append_conn_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool append_conn_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_append_conn_resp(&xdr, (struct append_conn_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool append_conn_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct append_conn_resp *_req = (struct append_conn_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct append_conn_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_append_conn_resp(&xdr, _req);
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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool pwd_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct pwd_req *_req = (struct pwd_req *)mp_malloc(g_mp,
		__func__, sizeof(struct pwd_req));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool pwd_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct pwd_resp *_req = (struct pwd_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct pwd_resp));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool ll_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct ll_req *_req = (struct ll_req *)mp_malloc(g_mp,
		__func__, sizeof(struct ll_req));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool ll_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct ll_resp *_req = (struct ll_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct ll_resp));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool cd_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct cd_req *_req = (struct cd_req *)mp_malloc(g_mp,
		__func__, sizeof(struct cd_req));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool cd_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct cd_resp *_req = (struct cd_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct cd_resp));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool get_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct get_req *_req = (struct get_req *)mp_malloc(g_mp,
		__func__, sizeof(struct get_req));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool get_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct get_resp *_req = (struct get_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct get_resp));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool put_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct put_req *_req = (struct put_req *)mp_malloc(g_mp,
		__func__, sizeof(struct put_req));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool put_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct put_resp *_req = (struct put_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct put_resp));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool common_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct common_resp *_req = (struct common_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct common_resp));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool write_msg_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct write_msg_req *_req = (struct write_msg_req *)mp_malloc(g_mp,
		__func__, sizeof(struct write_msg_req));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool read_msg_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct read_msg_req *_req = (struct read_msg_req *)mp_malloc(g_mp,
		__func__, sizeof(struct read_msg_req));

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
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool read_msg_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct read_msg_resp *_req = (struct read_msg_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct read_msg_resp));

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

bool mp_stat_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_mp_stat_req(&xdr, (struct mp_stat_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool mp_stat_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct mp_stat_req *_req = (struct mp_stat_req *)mp_malloc(g_mp,
		__func__, sizeof(struct mp_stat_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_mp_stat_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool mp_stat_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_mp_stat_resp(&xdr, (struct mp_stat_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool mp_stat_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct mp_stat_resp *_req = (struct mp_stat_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct mp_stat_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_mp_stat_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool directcmd_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_directcmd_req(&xdr, (struct directcmd_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool directcmd_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct directcmd_req *_req = (struct directcmd_req *)mp_malloc(g_mp,
		__func__, sizeof(struct directcmd_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_directcmd_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool directcmd_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_directcmd_resp(&xdr, (struct directcmd_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool directcmd_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct directcmd_resp *_req = (struct directcmd_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct directcmd_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_directcmd_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool who_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_who_req(&xdr, (struct who_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool who_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct who_req *_req = (struct who_req *)mp_malloc(g_mp,
		__func__, sizeof(struct who_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_who_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool who_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_who_resp(&xdr, (struct who_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool who_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct who_resp *_req = (struct who_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct who_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_who_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool write_req_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_write_req(&xdr, (struct write_req *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool write_req_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct write_req *_req = (struct write_req *)mp_malloc(g_mp,
		__func__, sizeof(struct write_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_write_req(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool write_resp_encode(void *req, unsigned char **buf, int *len)
{
	add_log(LOG_INFO, "%s: in", __func__);
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	int ret = xdr_write_resp(&xdr, (struct write_resp *)req);

	fclose(fp);
	*len = size;
	add_log(LOG_INFO, "%s: encode ret=%d, encode_len=%d",
		__func__, ret, *len);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}

bool write_resp_decode(unsigned char *buf, int len, void **req)
{
	add_log(LOG_INFO, "%s: in", __func__);
	struct write_resp *_req = (struct write_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct write_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	int ret = xdr_write_resp(&xdr, _req);
	fclose(fp);

	*req = _req;
	add_log(LOG_INFO, "%s: decode ret=%d", __func__, ret);
	add_log(LOG_INFO, "%s: out", __func__);

	return ret;
}
