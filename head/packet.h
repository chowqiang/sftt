#ifndef _PACKET_H_
#define _PACKET_H_

#include "config.h"

#define BLOCK_TYPE_SIZE			(sizeof(int))
#define PACKET_LEN_SIZE			(sizeof(int))	

enum user_validate_status {
	UVS_INVALID,
	UVS_PASS,
	UVS_BLOCK
};

enum packet_type {
	BLOCK_TYPE_VALIDATE,
	BLOCK_TYPE_FILE_NAME,
	BLOCK_TYPE_DATA,
	BLOCK_TYPE_FILE_END,
	BLOCK_TYPE_SEND_COMPLETE
};

typedef struct {
	int name_len;
	int passwd_len;
	char name[USER_NAME_MAX_LEN];
	char passwd_md5[MD5_LEN + 1];
} validate_req;

typedef struct {
	int status;
	uint64_t uid;
	char name[USER_NAME_MAX_LEN];
} validate_resp;

typedef struct sftt_packet {
	int type;
	int data_len;
	unsigned char *content;
	int block_size;
} sftt_packet;

#endif
