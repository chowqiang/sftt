#include "packet.h"
#include "serialize.h"
#include "serialize_handler.h"

struct serialize_handler serializables[] = {
	{PACKET_TYPE_VALIDATE_REQ, validate_req_encode, validate_req_decode},
	{PACKET_TYPE_VALIDATE_RSP, validate_resp_encode, validate_resp_decode},
	{PACKET_TYPE_PWD_REQ, pwd_req_encode, pwd_req_decode},
	{PACKET_TYPE_PWD_RSP, pwd_resp_encode, pwd_resp_decode},
	{PACKET_TYPE_CD_REQ, cd_req_encode, cd_req_decode},
	{PACKET_TYPE_CD_RSP, cd_resp_encode, cd_resp_decode},
	{-1, NULL, NULL},
};
