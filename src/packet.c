#include "req_rsp.h"

#define SET_PACKET_MIN_LEN(type, ltype) \
	int type##_REQ_PACKET_MIN_LEN = (sizeof(struct ltype##_req) * 10); \
	int type##_RESP_PACKET_MIN_LEN = (sizeof(struct ltype##_resp) * 10);

SET_PACKET_MIN_LEN(VALIDATE, validate)
SET_PACKET_MIN_LEN(PWD, pwd)
SET_PACKET_MIN_LEN(CD, cd)
SET_PACKET_MIN_LEN(LL, ll)
