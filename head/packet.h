#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdint.h>
#include "config.h"
#include "md5.h"
#include "session.h"

#define PACKET_TYPE_SIZE		(sizeof(int))
#define PACKET_LEN_SIZE			(sizeof(int))	

#define VALIDATE_PACKET_MIN_LEN		10240
#define PASSWD_MD5_LEN				(MD5_LEN + 1)

enum user_validate_status {
	UVS_INVALID,
	UVS_PASS,
	UVS_BLOCK
};

enum packet_type {
	PACKET_TYPE_VALIDATE_REQ,
	PACKET_TYPE_VALIDATE_RSP,
	PACKET_TYPE_FILE_NAME_REQ,
	PACKET_TYPE_FILE_NAME_RSP,
	PACKET_TYPE_DATA_REQ,
	PACKET_TYPE_DATA_RSP,
	PACKET_TYPE_FILE_END_REQ,
	PACKET_TYPE_FILE_END_RSP,
	PACKET_TYPE_SEND_COMPLETE_REQ,
	PACKET_TYPE_SEND_COMPLETE_RSP,
};


typedef struct sftt_packet {
	int type;
	void *obj;
	unsigned char *content;
	int data_len;
	int block_size;
} sftt_packet;

#endif
