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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "mem_pool.h"
#include "serialize.h"
#include "utils.h"

extern struct mem_pool *g_mp;

void test_validate_req(void)
{
	struct validate_req *req_info, *req_info2;

	req_info = mp_malloc(g_mp, __func__, sizeof(struct validate_req));
	assert(req_info != NULL);

	strcpy(req_info->name, "root");
	req_info->name_len = strlen(req_info->name);

	strcpy(req_info->passwd_md5, "19910930abc");
	req_info->passwd_len = strlen(req_info->passwd_md5);

	unsigned char *buf = NULL;
	int len = 0;
	bool ret = false;

	ret = validate_req_encode(req_info, &buf, &len);
	printf("encode ret = %d, buf = %p, len = %d\n", ret, buf, len);


	ret = validate_req_decode(buf, len, (void *)&req_info2);
	printf("decode ret = %d\n", ret);
	printf("name: %s, passwd: %s\n", req_info2->name, req_info2->passwd_md5);
}

void test_validate_resp(void)
{
	struct validate_resp *resp_info, *resp_info2;
	struct validate_resp_data *data, *data2;

	resp_info = mp_malloc(g_mp, __func__, sizeof(struct validate_resp));
	assert(resp_info != NULL);
	data = &resp_info->data;

	resp_info->status = 1;
	strcpy(data->name, "root");
	//resp_info->name_len = strlen(resp_info->name);
	data->uid = 2021;

	//strcpy(resp_info->passwd_md5, "19910930abc");
	//resp_info->passwd_len = strlen(resp_info->passwd_md5);
	char session_id[SESSION_ID_LEN];
	gen_session_id(session_id, SESSION_ID_LEN);
	strcpy(data->session_id, session_id);

	unsigned char *buf = NULL;
	int len = 0;
	bool ret = false;

	ret = validate_resp_encode(resp_info, &buf, &len);
	printf("encode ret = %d, buf = %p, len = %d\n", ret, buf, len);


	ret = validate_resp_decode(buf, len, (void *)&resp_info2);
	printf("decode ret = %d\n", ret);
	data2 = &resp_info2->data;
	printf("name: %s, uid: %ld\n", data2->name, data2->uid);
	printf("status: %d, session_id: %s\n", resp_info2->status, data2->session_id);

	mp_stat(g_mp);
	printf("sizeof int: %ld, sizeof long: %ld, sizeof short: %ld\n",
			sizeof(int), sizeof(long), sizeof(short));
	//printf("%c\n", 0x72);
}

void test_ll_resp(void)
{
	struct ll_resp resp = {
		.status = 0,
		.flags = REQ_RESP_FLAG_NONE,
		.message = "message",
		.data = {
			.total = 10,
			.this_nr = 3,
			.entries[0] = {"a.txt", 0, 1, 10, 20210606, 20210605, 20210604},
			.entries[1] = {"b.txt", 0, 1, 10, 20210606, 20210605, 20210604},
			.entries[2] = {"src", 0, 2, 10, 20210606, 20210605, 20210604},
		},
	};
	struct ll_resp *_resp = NULL;

	unsigned char *buf = NULL;
	bool ret = 0;
	int len = 0;

	ret = ll_resp_encode(&resp, &buf, &len);
	printf("ll resp encode: ret=%d, len=%d\n", ret, len);

	ret = ll_resp_decode(buf, len, (void **)&_resp);
	printf("ll resp decode: ret=%d, total=%d, this_nr=%d\n",
		ret, _resp->data.total, _resp->data.this_nr);
	printf("file list: %s, %s, %s\n", _resp->data.entries[0].name,
		_resp->data.entries[1].name, _resp->data.entries[2].name);
}

void test_ll_req(void)
{
	struct ll_req req = {
		"abcdefg",
		"/home/zhoumin"
	};
	struct ll_req *_req = NULL;

	unsigned char *buf = NULL;
	bool ret = 0;
	int len = 0;

	printf("path=%s\n", req.path);
	ret = ll_req_encode(&req, &buf, &len);
	printf("ll req encode: ret=%d, len=%d\n", ret, len);

	ret = ll_req_decode(buf, len, (void **)&_req);
	printf("ll req decode: ret=%d, path=%s\n", ret, _req->path);
}

void test_get_resp(void) {
	struct get_resp resp = {
		.status = 1,
		.flags = 0x1,
		.message = "ok",
		.data = {
			.total_files = 1,
			.file_idx = 0,
			.entry = {
				.type = 1,
				.total_size = 100,
				.this_size = 100,
				.mode = 0x111,
				.content = "ABCDEFG",
			}
		},
		.need_reply = 0,
	};

	struct get_resp *_resp = NULL;
	unsigned char *buf = NULL;
	bool ret = 0;
	int len = 0;

	printf("message: %s\n", resp.message);
	printf("content: %s\n", resp.data.entry.content);
	ret = get_resp_encode(&resp, &buf, &len);
	printf("get resp encode: ret=%d, len=%d\n", ret, len);

	ret = get_resp_decode(buf, len, (void **)&_resp);
	printf("get resp decode: ret=%d, len=%d\n", ret, len);
	printf("message: %s\n", _resp->message);
	printf("content: %s\n", _resp->data.entry.content);
}

void test_get_req(void)
{
	struct get_req req = {
		.session_id = "abcdefg1234",
		.path = "/root/sftt",
		.to_peer = 1,
		.user = {
			.session_id = "1234abcdefg",
			.name = "zhoumin",
			.ip = "192.168.1.81",
			.port = 1024
		}
	};

	struct get_req *_req = NULL;
	unsigned char *buf = NULL;
	bool ret = 0;
	int len = 0;

	printf("session_id: %s\n", req.session_id);
	printf("user.session_id: %s\n", req.user.session_id);
	ret = get_req_encode(&req, &buf, &len);
	printf("get req encode: ret=%d, len=%d\n", ret, len);

	ret = get_req_decode(buf, len, (void **)&_req);
	printf("get req decode: ret=%d, len=%d\n", ret, len);
	printf("session_id: %s\n", _req->session_id);
	printf("user.session_id: %s\n", _req->user.session_id);
}

void *thread_foo(void *arg)
{
	int i = 0;

	for (i = 0; i < 1000; ++i) {
		test_get_resp();
	}

	return NULL;
}

void test_get_resp_multi_thread(void)
{
	int i = 0;
	pthread_t thread_ids[2];
	void *res = NULL;

	for (i = 0; i < 2; ++i) {
		pthread_create(&thread_ids[i], NULL, thread_foo, NULL);
	}

	for (i = 0; i < 2; ++i) {
		pthread_join(thread_ids[i], &res);

	}
}

int main(void)
{
	test_validate_req();
	test_validate_resp();
	test_ll_req();
	test_ll_resp();

	test_get_resp_multi_thread();

	return 0;
}
