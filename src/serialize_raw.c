/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdio.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include "common.h"
#include "debug.h"
#include "log.h"
#include "mem_pool.h"
#include "serialize_raw.h"

extern struct mem_pool *g_mp;

bool channel_info_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_channel_info_req(&xdr, (struct channel_info_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool channel_info_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct channel_info_req *_req = (struct channel_info_req *)mp_malloc(g_mp,
		__func__, sizeof(struct channel_info_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_channel_info_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool channel_info_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_channel_info_resp(&xdr, (struct channel_info_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool channel_info_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct channel_info_resp *_req = (struct channel_info_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct channel_info_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_channel_info_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool validate_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_validate_req(&xdr, (struct validate_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool validate_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct validate_req *_req = (struct validate_req *)mp_malloc(g_mp,
		__func__, sizeof(struct validate_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_validate_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool validate_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_validate_resp(&xdr, (struct validate_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool validate_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct validate_resp *_req = (struct validate_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct validate_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_validate_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool append_conn_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_append_conn_req(&xdr, (struct append_conn_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool append_conn_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct append_conn_req *_req = (struct append_conn_req *)mp_malloc(g_mp,
		__func__, sizeof(struct append_conn_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_append_conn_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool append_conn_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_append_conn_resp(&xdr, (struct append_conn_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool append_conn_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct append_conn_resp *_req = (struct append_conn_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct append_conn_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_append_conn_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool pwd_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_pwd_req(&xdr, (struct pwd_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool pwd_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct pwd_req *_req = (struct pwd_req *)mp_malloc(g_mp,
		__func__, sizeof(struct pwd_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_pwd_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool pwd_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_pwd_resp(&xdr, (struct pwd_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool pwd_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct pwd_resp *_req = (struct pwd_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct pwd_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_pwd_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool ll_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_ll_req(&xdr, (struct ll_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool ll_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct ll_req *_req = (struct ll_req *)mp_malloc(g_mp,
		__func__, sizeof(struct ll_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_ll_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool ll_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_ll_resp(&xdr, (struct ll_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool ll_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct ll_resp *_req = (struct ll_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct ll_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_ll_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool cd_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_cd_req(&xdr, (struct cd_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool cd_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct cd_req *_req = (struct cd_req *)mp_malloc(g_mp,
		__func__, sizeof(struct cd_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_cd_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool cd_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_cd_resp(&xdr, (struct cd_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool cd_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct cd_resp *_req = (struct cd_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct cd_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_cd_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool get_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_get_req(&xdr, (struct get_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool get_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct get_req *_req = (struct get_req *)mp_malloc(g_mp,
		__func__, sizeof(struct get_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_get_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool get_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_get_resp(&xdr, (struct get_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool get_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct get_resp *_req = (struct get_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct get_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_get_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool put_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_put_req(&xdr, (struct put_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool put_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct put_req *_req = (struct put_req *)mp_malloc(g_mp,
		__func__, sizeof(struct put_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_put_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool put_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_put_resp(&xdr, (struct put_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool put_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct put_resp *_req = (struct put_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct put_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_put_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool common_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_common_resp(&xdr, (struct common_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool common_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct common_resp *_req = (struct common_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct common_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_common_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_msg_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_write_msg_req(&xdr, (struct write_msg_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_msg_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct write_msg_req *_req = (struct write_msg_req *)mp_malloc(g_mp,
		__func__, sizeof(struct write_msg_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_write_msg_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool read_msg_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_read_msg_req(&xdr, (struct read_msg_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool read_msg_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct read_msg_req *_req = (struct read_msg_req *)mp_malloc(g_mp,
		__func__, sizeof(struct read_msg_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_read_msg_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool read_msg_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_read_msg_resp(&xdr, (struct read_msg_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool read_msg_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct read_msg_resp *_req = (struct read_msg_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct read_msg_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_read_msg_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool mp_stat_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_mp_stat_req(&xdr, (struct mp_stat_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool mp_stat_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct mp_stat_req *_req = (struct mp_stat_req *)mp_malloc(g_mp,
		__func__, sizeof(struct mp_stat_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_mp_stat_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool mp_stat_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_mp_stat_resp(&xdr, (struct mp_stat_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool mp_stat_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct mp_stat_resp *_req = (struct mp_stat_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct mp_stat_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_mp_stat_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool directcmd_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_directcmd_req(&xdr, (struct directcmd_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool directcmd_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct directcmd_req *_req = (struct directcmd_req *)mp_malloc(g_mp,
		__func__, sizeof(struct directcmd_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_directcmd_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool directcmd_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_directcmd_resp(&xdr, (struct directcmd_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool directcmd_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct directcmd_resp *_req = (struct directcmd_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct directcmd_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_directcmd_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool who_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_who_req(&xdr, (struct who_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool who_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct who_req *_req = (struct who_req *)mp_malloc(g_mp,
		__func__, sizeof(struct who_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_who_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool who_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_who_resp(&xdr, (struct who_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool who_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct who_resp *_req = (struct who_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct who_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_who_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_write_req(&xdr, (struct write_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct write_req *_req = (struct write_req *)mp_malloc(g_mp,
		__func__, sizeof(struct write_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_write_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_write_resp(&xdr, (struct write_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct write_resp *_req = (struct write_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct write_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_write_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool port_update_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_port_update_req(&xdr, (struct port_update_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool port_update_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct port_update_req *_req = (struct port_update_req *)mp_malloc(g_mp,
		__func__, sizeof(struct port_update_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_port_update_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool port_update_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_port_update_resp(&xdr, (struct port_update_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool port_update_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct port_update_resp *_req = (struct port_update_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct port_update_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_port_update_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool reconnect_req_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_reconnect_req(&xdr, (struct reconnect_req *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool reconnect_req_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct reconnect_req *_req = (struct reconnect_req *)mp_malloc(g_mp,
		__func__, sizeof(struct reconnect_req));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_reconnect_req(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool reconnect_resp_raw_encode(void *req, unsigned char **buf, int *len)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	size_t size = 0;
	FILE *fp = open_memstream((char **)buf, &size);

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_ENCODE);

	bool ret = xdr_reconnect_resp(&xdr, (struct reconnect_resp *)req);

	fflush(fp);
	fclose(fp);

	*len = size;
	DEBUG((DEBUG_DEBUG, "raw_encode done|ret=%d|raw_encode_len=%d\n",
		ret, *len));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool reconnect_resp_raw_decode(unsigned char *buf, int len, void **req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));
	struct reconnect_resp *_req = (struct reconnect_resp *)mp_malloc(g_mp,
		__func__, sizeof(struct reconnect_resp));

	FILE *fp = fmemopen(buf, len, "r");

	XDR xdr;
	xdrstdio_create(&xdr, fp, XDR_DECODE);

	bool ret = xdr_reconnect_resp(&xdr, _req);
	fflush(fp);
	fclose(fp);

	*req = _req;
	DEBUG((DEBUG_DEBUG, "raw_decode done|ret=%d\n", ret));
	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}
