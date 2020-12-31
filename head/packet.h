#ifndef _PACKET_H_
#define _PACKET_H_

#define BLOCK_TYPE_SIZE			(sizeof(int))
#define PACKET_LEN_SIZE			(sizeof(int))	

enum packet_type {
	BLOCK_TYPE_FILE_NAME,
	BLOCK_TYPE_DATA,
	BLOCK_TYPE_FILE_END,
	BLOCK_TYPE_SEND_COMPLETE
};

typedef struct sftt_packet {
	int type;
	int data_len;
	unsigned char *content;
	int block_size;
} sftt_packet;

#endif
