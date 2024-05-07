/*
 * Automatically generated - do not edit
 */

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include "common.h"
#include "debug.h"
#include "log.h"
#include "mem_pool.h"
#include "serialize.h"

bool version_info_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct version_info *_req = req;

	_req->major = htons(_req->major);
	_req->minor = htons(_req->minor);
	_req->revision = htons(_req->revision);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct version_info));
		memcpy(*buf, req, sizeof(struct version_info));
		*len = sizeof(struct version_info);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __version_info_raw_decode(struct version_info *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	struct version_info *_req = req;

	_req->major = ntohs(_req->major);
	_req->minor = ntohs(_req->minor);
	_req->revision = ntohs(_req->revision);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool version_info_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct version_info *_req = (struct version_info *)mp_malloc(g_mp, __func__,
		sizeof(struct version_info));

	memcpy(_req, buf, len);
	_req->major = ntohs(_req->major);
	_req->minor = ntohs(_req->minor);
	_req->revision = ntohs(_req->revision);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool channel_info_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct channel_info_req *_req = req;

	_req->flags = htonl(_req->flags);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct channel_info_req));
		memcpy(*buf, req, sizeof(struct channel_info_req));
		*len = sizeof(struct channel_info_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __channel_info_req_raw_decode(struct channel_info_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	struct channel_info_req *_req = req;

	_req->flags = ntohl(_req->flags);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool channel_info_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct channel_info_req *_req = (struct channel_info_req *)mp_malloc(g_mp, __func__,
		sizeof(struct channel_info_req));

	memcpy(_req, buf, len);
	_req->flags = ntohl(_req->flags);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool channel_info_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct channel_info_resp_data *_req = req;

	_req->main_port = htonl(_req->main_port);
	_req->second_port = htonl(_req->second_port);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct channel_info_resp_data));
		memcpy(*buf, req, sizeof(struct channel_info_resp_data));
		*len = sizeof(struct channel_info_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __channel_info_resp_data_raw_decode(struct channel_info_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	struct channel_info_resp_data *_req = req;

	_req->main_port = ntohl(_req->main_port);
	_req->second_port = ntohl(_req->second_port);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool channel_info_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct channel_info_resp_data *_req = (struct channel_info_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct channel_info_resp_data));

	memcpy(_req, buf, len);
	_req->main_port = ntohl(_req->main_port);
	_req->second_port = ntohl(_req->second_port);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool channel_info_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct channel_info_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	channel_info_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct channel_info_resp));
		memcpy(*buf, req, sizeof(struct channel_info_resp));
		*len = sizeof(struct channel_info_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __channel_info_resp_raw_decode(struct channel_info_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct channel_info_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__channel_info_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool channel_info_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct channel_info_resp *_req = (struct channel_info_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct channel_info_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__channel_info_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool validate_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct validate_req *_req = req;

	_req->name_len = htonl(_req->name_len);
	_req->passwd_len = htonl(_req->passwd_len);
	version_info_raw_encode(&_req->ver, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct validate_req));
		memcpy(*buf, req, sizeof(struct validate_req));
		*len = sizeof(struct validate_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __validate_req_raw_decode(struct validate_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct validate_req *_req = req;

	_req->name_len = ntohl(_req->name_len);
	_req->passwd_len = ntohl(_req->passwd_len);
	__version_info_raw_decode(&_req->ver);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool validate_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct validate_req *_req = (struct validate_req *)mp_malloc(g_mp, __func__,
		sizeof(struct validate_req));

	memcpy(_req, buf, len);
	_req->name_len = ntohl(_req->name_len);
	_req->passwd_len = ntohl(_req->passwd_len);
	__version_info_raw_decode(&_req->ver);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool validate_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct validate_resp_data *_req = req;

	_req->uid = htonl(_req->uid);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct validate_resp_data));
		memcpy(*buf, req, sizeof(struct validate_resp_data));
		*len = sizeof(struct validate_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __validate_resp_data_raw_decode(struct validate_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct validate_resp_data *_req = req;

	_req->uid = ntohl(_req->uid);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool validate_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct validate_resp_data *_req = (struct validate_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct validate_resp_data));

	memcpy(_req, buf, len);
	_req->uid = ntohl(_req->uid);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool validate_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct validate_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	validate_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct validate_resp));
		memcpy(*buf, req, sizeof(struct validate_resp));
		*len = sizeof(struct validate_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __validate_resp_raw_decode(struct validate_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct validate_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__validate_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool validate_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct validate_resp *_req = (struct validate_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct validate_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__validate_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool append_conn_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct append_conn_req *_req = req;

	_req->type = htonl(_req->type);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct append_conn_req));
		memcpy(*buf, req, sizeof(struct append_conn_req));
		*len = sizeof(struct append_conn_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __append_conn_req_raw_decode(struct append_conn_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct append_conn_req *_req = req;

	_req->type = ntohl(_req->type);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool append_conn_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct append_conn_req *_req = (struct append_conn_req *)mp_malloc(g_mp, __func__,
		sizeof(struct append_conn_req));

	memcpy(_req, buf, len);
	_req->type = ntohl(_req->type);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool append_conn_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct append_conn_resp_data *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct append_conn_resp_data));
		memcpy(*buf, req, sizeof(struct append_conn_resp_data));
		*len = sizeof(struct append_conn_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __append_conn_resp_data_raw_decode(struct append_conn_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct append_conn_resp_data *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool append_conn_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct append_conn_resp_data *_req = (struct append_conn_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct append_conn_resp_data));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool append_conn_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct append_conn_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	append_conn_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct append_conn_resp));
		memcpy(*buf, req, sizeof(struct append_conn_resp));
		*len = sizeof(struct append_conn_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __append_conn_resp_raw_decode(struct append_conn_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct append_conn_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__append_conn_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool append_conn_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct append_conn_resp *_req = (struct append_conn_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct append_conn_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__append_conn_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool logged_in_user_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct logged_in_user *_req = req;

	_req->port = htonl(_req->port);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct logged_in_user));
		memcpy(*buf, req, sizeof(struct logged_in_user));
		*len = sizeof(struct logged_in_user);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __logged_in_user_raw_decode(struct logged_in_user *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct logged_in_user *_req = req;

	_req->port = ntohl(_req->port);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool logged_in_user_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct logged_in_user *_req = (struct logged_in_user *)mp_malloc(g_mp, __func__,
		sizeof(struct logged_in_user));

	memcpy(_req, buf, len);
	_req->port = ntohl(_req->port);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool pwd_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct pwd_req *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct pwd_req));
		memcpy(*buf, req, sizeof(struct pwd_req));
		*len = sizeof(struct pwd_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __pwd_req_raw_decode(struct pwd_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct pwd_req *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool pwd_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct pwd_req *_req = (struct pwd_req *)mp_malloc(g_mp, __func__,
		sizeof(struct pwd_req));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool pwd_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct pwd_resp_data *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct pwd_resp_data));
		memcpy(*buf, req, sizeof(struct pwd_resp_data));
		*len = sizeof(struct pwd_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __pwd_resp_data_raw_decode(struct pwd_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct pwd_resp_data *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool pwd_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct pwd_resp_data *_req = (struct pwd_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct pwd_resp_data));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool pwd_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct pwd_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	pwd_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct pwd_resp));
		memcpy(*buf, req, sizeof(struct pwd_resp));
		*len = sizeof(struct pwd_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __pwd_resp_raw_decode(struct pwd_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct pwd_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__pwd_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool pwd_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct pwd_resp *_req = (struct pwd_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct pwd_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__pwd_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool ll_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct ll_req *_req = req;

	_req->to_peer = htonl(_req->to_peer);
	logged_in_user_raw_encode(&_req->user, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct ll_req));
		memcpy(*buf, req, sizeof(struct ll_req));
		*len = sizeof(struct ll_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __ll_req_raw_decode(struct ll_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct ll_req *_req = req;

	_req->to_peer = ntohl(_req->to_peer);
	__logged_in_user_raw_decode(&_req->user);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool ll_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct ll_req *_req = (struct ll_req *)mp_malloc(g_mp, __func__,
		sizeof(struct ll_req));

	memcpy(_req, buf, len);
	_req->to_peer = ntohl(_req->to_peer);
	__logged_in_user_raw_decode(&_req->user);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool file_entry_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct file_entry *_req = req;

	_req->type = htonl(_req->type);
	_req->size = htonl(_req->size);
	_req->c_time = htonl(_req->c_time);
	_req->a_time = htonl(_req->a_time);
	_req->m_time = htonl(_req->m_time);
	_req->mode = htonl(_req->mode);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct file_entry));
		memcpy(*buf, req, sizeof(struct file_entry));
		*len = sizeof(struct file_entry);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __file_entry_raw_decode(struct file_entry *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct file_entry *_req = req;

	_req->type = ntohl(_req->type);
	_req->size = ntohl(_req->size);
	_req->c_time = ntohl(_req->c_time);
	_req->a_time = ntohl(_req->a_time);
	_req->m_time = ntohl(_req->m_time);
	_req->mode = ntohl(_req->mode);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool file_entry_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct file_entry *_req = (struct file_entry *)mp_malloc(g_mp, __func__,
		sizeof(struct file_entry));

	memcpy(_req, buf, len);
	_req->type = ntohl(_req->type);
	_req->size = ntohl(_req->size);
	_req->c_time = ntohl(_req->c_time);
	_req->a_time = ntohl(_req->a_time);
	_req->m_time = ntohl(_req->m_time);
	_req->mode = ntohl(_req->mode);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool ll_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct ll_resp_data *_req = req;

	_req->total = htonl(_req->total);
	_req->this_nr = htonl(_req->this_nr);
	for (i = 0; i < XDR_FILE_ENTRY_MAX_CNT; ++i) {
		file_entry_raw_encode(&_req->entries[i], NULL, NULL, NULL);
	}

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct ll_resp_data));
		memcpy(*buf, req, sizeof(struct ll_resp_data));
		*len = sizeof(struct ll_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __ll_resp_data_raw_decode(struct ll_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct ll_resp_data *_req = req;

	_req->total = ntohl(_req->total);
	_req->this_nr = ntohl(_req->this_nr);
	for (i = 0; i < XDR_FILE_ENTRY_MAX_CNT; ++i) {
		__file_entry_raw_decode(&_req->entries[i]);
	}

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool ll_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct ll_resp_data *_req = (struct ll_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct ll_resp_data));

	memcpy(_req, buf, len);
	_req->total = ntohl(_req->total);
	_req->this_nr = ntohl(_req->this_nr);
	for (i = 0; i < XDR_FILE_ENTRY_MAX_CNT; ++i) {
		__file_entry_raw_decode(&_req->entries[i]);
	}

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool ll_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct ll_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	ll_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct ll_resp));
		memcpy(*buf, req, sizeof(struct ll_resp));
		*len = sizeof(struct ll_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __ll_resp_raw_decode(struct ll_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct ll_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__ll_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool ll_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct ll_resp *_req = (struct ll_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct ll_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__ll_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool cd_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct cd_req *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct cd_req));
		memcpy(*buf, req, sizeof(struct cd_req));
		*len = sizeof(struct cd_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __cd_req_raw_decode(struct cd_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct cd_req *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool cd_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct cd_req *_req = (struct cd_req *)mp_malloc(g_mp, __func__,
		sizeof(struct cd_req));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool cd_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct cd_resp_data *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct cd_resp_data));
		memcpy(*buf, req, sizeof(struct cd_resp_data));
		*len = sizeof(struct cd_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __cd_resp_data_raw_decode(struct cd_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct cd_resp_data *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool cd_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct cd_resp_data *_req = (struct cd_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct cd_resp_data));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool cd_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct cd_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	cd_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct cd_resp));
		memcpy(*buf, req, sizeof(struct cd_resp));
		*len = sizeof(struct cd_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __cd_resp_raw_decode(struct cd_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct cd_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__cd_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool cd_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct cd_resp *_req = (struct cd_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct cd_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__cd_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool get_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct get_req *_req = req;

	_req->to_peer = htonl(_req->to_peer);
	logged_in_user_raw_encode(&_req->user, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct get_req));
		memcpy(*buf, req, sizeof(struct get_req));
		*len = sizeof(struct get_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __get_req_raw_decode(struct get_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct get_req *_req = req;

	_req->to_peer = ntohl(_req->to_peer);
	__logged_in_user_raw_decode(&_req->user);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool get_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct get_req *_req = (struct get_req *)mp_malloc(g_mp, __func__,
		sizeof(struct get_req));

	memcpy(_req, buf, len);
	_req->to_peer = ntohl(_req->to_peer);
	__logged_in_user_raw_decode(&_req->user);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool trans_entry_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct trans_entry *_req = req;

	_req->type = htonl(_req->type);
	_req->this_size = htonl(_req->this_size);
	_req->mode = htonl(_req->mode);
	_req->total_size = htonl(_req->total_size);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct trans_entry));
		memcpy(*buf, req, sizeof(struct trans_entry));
		*len = sizeof(struct trans_entry);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __trans_entry_raw_decode(struct trans_entry *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct trans_entry *_req = req;

	_req->type = ntohl(_req->type);
	_req->this_size = ntohl(_req->this_size);
	_req->mode = ntohl(_req->mode);
	_req->total_size = ntohl(_req->total_size);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool trans_entry_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct trans_entry *_req = (struct trans_entry *)mp_malloc(g_mp, __func__,
		sizeof(struct trans_entry));

	memcpy(_req, buf, len);
	_req->type = ntohl(_req->type);
	_req->this_size = ntohl(_req->this_size);
	_req->mode = ntohl(_req->mode);
	_req->total_size = ntohl(_req->total_size);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool get_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct get_resp_data *_req = req;

	_req->total_files = htonl(_req->total_files);
	_req->file_idx = htonl(_req->file_idx);
	trans_entry_raw_encode(&_req->entry, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct get_resp_data));
		memcpy(*buf, req, sizeof(struct get_resp_data));
		*len = sizeof(struct get_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __get_resp_data_raw_decode(struct get_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	struct get_resp_data *_req = req;

	_req->total_files = ntohl(_req->total_files);
	_req->file_idx = ntohl(_req->file_idx);
	__trans_entry_raw_decode(&_req->entry);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool get_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct get_resp_data *_req = (struct get_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct get_resp_data));

	memcpy(_req, buf, len);
	_req->total_files = ntohl(_req->total_files);
	_req->file_idx = ntohl(_req->file_idx);
	__trans_entry_raw_decode(&_req->entry);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool get_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct get_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	get_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);
	_req->need_reply = htonl(_req->need_reply);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct get_resp));
		memcpy(*buf, req, sizeof(struct get_resp));
		*len = sizeof(struct get_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __get_resp_raw_decode(struct get_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct get_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__get_resp_data_raw_decode(&_req->data);
	_req->need_reply = ntohl(_req->need_reply);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool get_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct get_resp *_req = (struct get_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct get_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__get_resp_data_raw_decode(&_req->data);
	_req->need_reply = ntohl(_req->need_reply);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool put_req_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct put_req_data *_req = req;

	_req->total_files = htonl(_req->total_files);
	_req->file_idx = htonl(_req->file_idx);
	trans_entry_raw_encode(&_req->entry, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct put_req_data));
		memcpy(*buf, req, sizeof(struct put_req_data));
		*len = sizeof(struct put_req_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __put_req_data_raw_decode(struct put_req_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	struct put_req_data *_req = req;

	_req->total_files = ntohl(_req->total_files);
	_req->file_idx = ntohl(_req->file_idx);
	__trans_entry_raw_decode(&_req->entry);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool put_req_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct put_req_data *_req = (struct put_req_data *)mp_malloc(g_mp, __func__,
		sizeof(struct put_req_data));

	memcpy(_req, buf, len);
	_req->total_files = ntohl(_req->total_files);
	_req->file_idx = ntohl(_req->file_idx);
	__trans_entry_raw_decode(&_req->entry);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool put_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct put_req *_req = req;

	_req->to_peer = htonl(_req->to_peer);
	logged_in_user_raw_encode(&_req->user, NULL, NULL, NULL);
	put_req_data_raw_encode(&_req->data, NULL, NULL, NULL);
	_req->flags = htonl(_req->flags);
	_req->need_reply = htonl(_req->need_reply);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct put_req));
		memcpy(*buf, req, sizeof(struct put_req));
		*len = sizeof(struct put_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __put_req_raw_decode(struct put_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct put_req *_req = req;

	_req->to_peer = ntohl(_req->to_peer);
	__logged_in_user_raw_decode(&_req->user);
	__put_req_data_raw_decode(&_req->data);
	_req->flags = ntohl(_req->flags);
	_req->need_reply = ntohl(_req->need_reply);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool put_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct put_req *_req = (struct put_req *)mp_malloc(g_mp, __func__,
		sizeof(struct put_req));

	memcpy(_req, buf, len);
	_req->to_peer = ntohl(_req->to_peer);
	__logged_in_user_raw_decode(&_req->user);
	__put_req_data_raw_decode(&_req->data);
	_req->flags = ntohl(_req->flags);
	_req->need_reply = ntohl(_req->need_reply);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool put_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct put_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct put_resp));
		memcpy(*buf, req, sizeof(struct put_resp));
		*len = sizeof(struct put_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __put_resp_raw_decode(struct put_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct put_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool put_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct put_resp *_req = (struct put_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct put_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool common_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct common_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct common_resp));
		memcpy(*buf, req, sizeof(struct common_resp));
		*len = sizeof(struct common_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __common_resp_raw_decode(struct common_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct common_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool common_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct common_resp *_req = (struct common_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct common_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_msg_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct write_msg_req *_req = req;

	_req->mtype = htonl(_req->mtype);
	_req->pid = htonl(_req->pid);
	_req->length = htonl(_req->length);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct write_msg_req));
		memcpy(*buf, req, sizeof(struct write_msg_req));
		*len = sizeof(struct write_msg_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __write_msg_req_raw_decode(struct write_msg_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct write_msg_req *_req = req;

	_req->mtype = ntohl(_req->mtype);
	_req->pid = ntohl(_req->pid);
	_req->length = ntohl(_req->length);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool write_msg_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct write_msg_req *_req = (struct write_msg_req *)mp_malloc(g_mp, __func__,
		sizeof(struct write_msg_req));

	memcpy(_req, buf, len);
	_req->mtype = ntohl(_req->mtype);
	_req->pid = ntohl(_req->pid);
	_req->length = ntohl(_req->length);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool read_msg_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct read_msg_req *_req = req;

	_req->mtype = htonl(_req->mtype);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct read_msg_req));
		memcpy(*buf, req, sizeof(struct read_msg_req));
		*len = sizeof(struct read_msg_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __read_msg_req_raw_decode(struct read_msg_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	struct read_msg_req *_req = req;

	_req->mtype = ntohl(_req->mtype);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool read_msg_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct read_msg_req *_req = (struct read_msg_req *)mp_malloc(g_mp, __func__,
		sizeof(struct read_msg_req));

	memcpy(_req, buf, len);
	_req->mtype = ntohl(_req->mtype);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool read_msg_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct read_msg_resp_data *_req = req;

	_req->mtype = htonl(_req->mtype);
	_req->pid = htonl(_req->pid);
	_req->length = htonl(_req->length);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct read_msg_resp_data));
		memcpy(*buf, req, sizeof(struct read_msg_resp_data));
		*len = sizeof(struct read_msg_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __read_msg_resp_data_raw_decode(struct read_msg_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct read_msg_resp_data *_req = req;

	_req->mtype = ntohl(_req->mtype);
	_req->pid = ntohl(_req->pid);
	_req->length = ntohl(_req->length);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool read_msg_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct read_msg_resp_data *_req = (struct read_msg_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct read_msg_resp_data));

	memcpy(_req, buf, len);
	_req->mtype = ntohl(_req->mtype);
	_req->pid = ntohl(_req->pid);
	_req->length = ntohl(_req->length);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool read_msg_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct read_msg_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	read_msg_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct read_msg_resp));
		memcpy(*buf, req, sizeof(struct read_msg_resp));
		*len = sizeof(struct read_msg_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __read_msg_resp_raw_decode(struct read_msg_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct read_msg_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__read_msg_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool read_msg_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct read_msg_resp *_req = (struct read_msg_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct read_msg_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__read_msg_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool mp_stat_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct mp_stat_req *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct mp_stat_req));
		memcpy(*buf, req, sizeof(struct mp_stat_req));
		*len = sizeof(struct mp_stat_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __mp_stat_req_raw_decode(struct mp_stat_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct mp_stat_req *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool mp_stat_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct mp_stat_req *_req = (struct mp_stat_req *)mp_malloc(g_mp, __func__,
		sizeof(struct mp_stat_req));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool mp_stat_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct mp_stat_resp_data *_req = req;

	_req->total_size = htonl(_req->total_size);
	_req->total_nodes = htonl(_req->total_nodes);
	_req->using_nodes = htonl(_req->using_nodes);
	_req->free_nodes = htonl(_req->free_nodes);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct mp_stat_resp_data));
		memcpy(*buf, req, sizeof(struct mp_stat_resp_data));
		*len = sizeof(struct mp_stat_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __mp_stat_resp_data_raw_decode(struct mp_stat_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	struct mp_stat_resp_data *_req = req;

	_req->total_size = ntohl(_req->total_size);
	_req->total_nodes = ntohl(_req->total_nodes);
	_req->using_nodes = ntohl(_req->using_nodes);
	_req->free_nodes = ntohl(_req->free_nodes);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool mp_stat_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct mp_stat_resp_data *_req = (struct mp_stat_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct mp_stat_resp_data));

	memcpy(_req, buf, len);
	_req->total_size = ntohl(_req->total_size);
	_req->total_nodes = ntohl(_req->total_nodes);
	_req->using_nodes = ntohl(_req->using_nodes);
	_req->free_nodes = ntohl(_req->free_nodes);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool mp_stat_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct mp_stat_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	mp_stat_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct mp_stat_resp));
		memcpy(*buf, req, sizeof(struct mp_stat_resp));
		*len = sizeof(struct mp_stat_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __mp_stat_resp_raw_decode(struct mp_stat_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct mp_stat_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__mp_stat_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool mp_stat_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct mp_stat_resp *_req = (struct mp_stat_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct mp_stat_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__mp_stat_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool directcmd_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct directcmd_req *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct directcmd_req));
		memcpy(*buf, req, sizeof(struct directcmd_req));
		*len = sizeof(struct directcmd_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __directcmd_req_raw_decode(struct directcmd_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct directcmd_req *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool directcmd_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct directcmd_req *_req = (struct directcmd_req *)mp_malloc(g_mp, __func__,
		sizeof(struct directcmd_req));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool directcmd_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct directcmd_resp_data *_req = req;

	_req->total_len = htonl(_req->total_len);
	_req->this_len = htonl(_req->this_len);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct directcmd_resp_data));
		memcpy(*buf, req, sizeof(struct directcmd_resp_data));
		*len = sizeof(struct directcmd_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __directcmd_resp_data_raw_decode(struct directcmd_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct directcmd_resp_data *_req = req;

	_req->total_len = ntohl(_req->total_len);
	_req->this_len = ntohl(_req->this_len);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool directcmd_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct directcmd_resp_data *_req = (struct directcmd_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct directcmd_resp_data));

	memcpy(_req, buf, len);
	_req->total_len = ntohl(_req->total_len);
	_req->this_len = ntohl(_req->this_len);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool directcmd_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct directcmd_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	directcmd_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct directcmd_resp));
		memcpy(*buf, req, sizeof(struct directcmd_resp));
		*len = sizeof(struct directcmd_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __directcmd_resp_raw_decode(struct directcmd_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct directcmd_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__directcmd_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool directcmd_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct directcmd_resp *_req = (struct directcmd_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct directcmd_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__directcmd_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool who_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct who_req *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct who_req));
		memcpy(*buf, req, sizeof(struct who_req));
		*len = sizeof(struct who_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __who_req_raw_decode(struct who_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct who_req *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool who_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct who_req *_req = (struct who_req *)mp_malloc(g_mp, __func__,
		sizeof(struct who_req));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool who_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct who_resp_data *_req = req;

	_req->total = htonl(_req->total);
	_req->this_nr = htonl(_req->this_nr);
	for (i = 0; i < XDR_LOGGED_IN_USER_MAX_CNT; ++i) {
		logged_in_user_raw_encode(&_req->users[i], NULL, NULL, NULL);
	}

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct who_resp_data));
		memcpy(*buf, req, sizeof(struct who_resp_data));
		*len = sizeof(struct who_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __who_resp_data_raw_decode(struct who_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct who_resp_data *_req = req;

	_req->total = ntohl(_req->total);
	_req->this_nr = ntohl(_req->this_nr);
	for (i = 0; i < XDR_LOGGED_IN_USER_MAX_CNT; ++i) {
		__logged_in_user_raw_decode(&_req->users[i]);
	}

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool who_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct who_resp_data *_req = (struct who_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct who_resp_data));

	memcpy(_req, buf, len);
	_req->total = ntohl(_req->total);
	_req->this_nr = ntohl(_req->this_nr);
	for (i = 0; i < XDR_LOGGED_IN_USER_MAX_CNT; ++i) {
		__logged_in_user_raw_decode(&_req->users[i]);
	}

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool who_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct who_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	who_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct who_resp));
		memcpy(*buf, req, sizeof(struct who_resp));
		*len = sizeof(struct who_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __who_resp_raw_decode(struct who_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct who_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__who_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool who_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct who_resp *_req = (struct who_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct who_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__who_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct write_req *_req = req;

	logged_in_user_raw_encode(&_req->user, NULL, NULL, NULL);
	_req->len = htonl(_req->len);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct write_req));
		memcpy(*buf, req, sizeof(struct write_req));
		*len = sizeof(struct write_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __write_req_raw_decode(struct write_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct write_req *_req = req;

	__logged_in_user_raw_decode(&_req->user);
	_req->len = ntohl(_req->len);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool write_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct write_req *_req = (struct write_req *)mp_malloc(g_mp, __func__,
		sizeof(struct write_req));

	memcpy(_req, buf, len);
	__logged_in_user_raw_decode(&_req->user);
	_req->len = ntohl(_req->len);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_resp_data_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct write_resp_data *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct write_resp_data));
		memcpy(*buf, req, sizeof(struct write_resp_data));
		*len = sizeof(struct write_resp_data);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __write_resp_data_raw_decode(struct write_resp_data *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct write_resp_data *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool write_resp_data_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct write_resp_data *_req = (struct write_resp_data *)mp_malloc(g_mp, __func__,
		sizeof(struct write_resp_data));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool write_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct write_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);
	write_resp_data_raw_encode(&_req->data, NULL, NULL, NULL);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct write_resp));
		memcpy(*buf, req, sizeof(struct write_resp));
		*len = sizeof(struct write_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __write_resp_raw_decode(struct write_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct write_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__write_resp_data_raw_decode(&_req->data);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool write_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct write_resp *_req = (struct write_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct write_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);
	__write_resp_data_raw_decode(&_req->data);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool port_update_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct port_update_req *_req = req;

	_req->second_port = htonl(_req->second_port);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct port_update_req));
		memcpy(*buf, req, sizeof(struct port_update_req));
		*len = sizeof(struct port_update_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __port_update_req_raw_decode(struct port_update_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	struct port_update_req *_req = req;

	_req->second_port = ntohl(_req->second_port);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool port_update_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	struct port_update_req *_req = (struct port_update_req *)mp_malloc(g_mp, __func__,
		sizeof(struct port_update_req));

	memcpy(_req, buf, len);
	_req->second_port = ntohl(_req->second_port);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool port_update_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct port_update_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct port_update_resp));
		memcpy(*buf, req, sizeof(struct port_update_resp));
		*len = sizeof(struct port_update_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __port_update_resp_raw_decode(struct port_update_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct port_update_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool port_update_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct port_update_resp *_req = (struct port_update_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct port_update_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool reconnect_req_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct reconnect_req *_req = req;

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct reconnect_req));
		memcpy(*buf, req, sizeof(struct reconnect_req));
		*len = sizeof(struct reconnect_req);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __reconnect_req_raw_decode(struct reconnect_req *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct reconnect_req *_req = req;

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool reconnect_req_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct reconnect_req *_req = (struct reconnect_req *)mp_malloc(g_mp, __func__,
		sizeof(struct reconnect_req));

	memcpy(_req, buf, len);
	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

bool reconnect_resp_raw_encode(void *req, unsigned char **buf, int *len,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct reconnect_resp *_req = req;

	_req->status = htonl(_req->status);
	_req->flags = htonl(_req->flags);

	if (buf && len) {
		*buf = mp_malloc(g_mp, __func__, sizeof(struct reconnect_resp));
		memcpy(*buf, req, sizeof(struct reconnect_resp));
		*len = sizeof(struct reconnect_resp);
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

void __reconnect_resp_raw_decode(struct reconnect_resp *req)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	int i = 0;
	struct reconnect_resp *_req = req;

	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);

	DEBUG((DEBUG_DEBUG, "out\n"));
}

bool reconnect_resp_raw_decode(unsigned char *buf, int len, void **req,
			enum free_mode *mode)
{
	DEBUG((DEBUG_DEBUG, "in\n"));

	bool ret = true;
	int i = 0;
	struct reconnect_resp *_req = (struct reconnect_resp *)mp_malloc(g_mp, __func__,
		sizeof(struct reconnect_resp));

	memcpy(_req, buf, len);
	_req->status = ntohl(_req->status);
	_req->flags = ntohl(_req->flags);

	if (req) {
		*req = _req;
		*mode = FREE_MODE_MP_FREE;
	}

	DEBUG((DEBUG_DEBUG, "out\n"));

	return ret;
}

