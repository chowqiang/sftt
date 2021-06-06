#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem_pool.h"
#include "req_rsp.h"
#include "serialize.h"
#include "utils.h"

extern struct mem_pool *g_mp;

void test_validate_req(void)
{
	struct validate_req *req_info, *req_info2;

	req_info = malloc(sizeof(struct validate_req));
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

	resp_info = malloc(sizeof(struct validate_resp));
	assert(resp_info != NULL);

	resp_info->status = 1;
	strcpy(resp_info->name, "root");
	//resp_info->name_len = strlen(resp_info->name);
	resp_info->uid = 2021;

	//strcpy(resp_info->passwd_md5, "19910930abc");
	//resp_info->passwd_len = strlen(resp_info->passwd_md5);
	char session_id[SESSION_ID_LEN];
	gen_session_id(session_id, SESSION_ID_LEN);
	strcpy(resp_info->session_id, session_id);

	unsigned char *buf = NULL;
	int len = 0;
	bool ret = false;

	ret = validate_resp_encode(resp_info, &buf, &len);
	printf("encode ret = %d, buf = %p, len = %d\n", ret, buf, len);

	
	ret = validate_resp_decode(buf, len, (void *)&resp_info2);
	printf("decode ret = %d\n", ret);
	printf("name: %s, uid: %d\n", resp_info2->name, resp_info2->uid);
	printf("status: %d, session_id: %s\n", resp_info2->status, resp_info2->session_id);

	mp_stat(g_mp);
	printf("sizeof int: %d, sizeof long: %d, sizeof short: %d\n", 
			sizeof(int), sizeof(long), sizeof(short));
	//printf("%c\n", 0x72);
}

void test_ll_resp(void)
{
	struct ll_resp resp = {
		3,
		{{"a.txt", 1, 10, 20210606, 20210605, 20210604},
		 {"b.txt", 1, 10, 20210603, 20210602, 20210601},
		 {"src", 2, 10, 20210503, 20210502, 20210501},
		},
		2
	};
	struct ll_resp *_resp = NULL;

	unsigned char *buf = NULL;
	bool ret = 0;
	int len = 0;

	ret = ll_resp_encode(&resp, &buf, &len);
	printf("ll resp encode: ret=%d, len=%d\n", ret, len);

	ret = ll_resp_decode(buf, len, (void **)&_resp);
	printf("ll resp decode: ret=%d, nr=%d, idx=%d\n", ret, _resp->nr, _resp->idx);
	printf("file list: %s, %s, %s\n", _resp->entries[0].name, _resp->entries[1].name,
			_resp->entries[2].name);
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

int main(void)
{
	test_validate_req();
	test_validate_resp();
	test_ll_req();
	test_ll_resp();

	return 0;
}
