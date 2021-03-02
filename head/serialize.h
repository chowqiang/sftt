#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include "packet.h"

bool validate_req_encode(void *req, unsigned char **buf, int *len);
bool validate_req_decode(unsigned char *buf, int len, void **req);
bool validate_rsp_encode(void *rsp, unsigned char **buf, int *len);
bool validate_rsp_decode(unsigned char *buf, int len, void **rsp);

bool send_file_name_req_encode(void *req, unsigned char **buf, int *len);
bool send_file_name_req_decode(unsigned char *buf, int len, void **req);
bool send_file_name_rsp_encode(void *rsp, unsigned char **buf, int *len);
bool send_file_name_rsp_decode(unsigned char *buf, int len, void **rsp);

bool send_data_req_encode(void *req, unsigned char **buf, int *len);
bool send_data_req_decode(unsigned char *buf, int len, void **req);
bool send_data_rsp_encode(void *rsp, unsigned char **buf, int *len);
bool send_data_rsp_decode(unsigned char *buf, int len, void **rsp);

bool send_file_end_req_encode(void *req, unsigned char **buf, int *len);
bool send_file_end_req_decode(unsigned char *buf, int len, void **req);
bool send_file_end_rsp_encode(void *rsp, unsigned char **buf, int *len);
bool send_file_end_rsp_decode(unsigned char *buf, int len, void **rsp);

bool send_end_complete_req_encode(void *req, unsigned char **buf, int *len);
bool send_end_complete_req_decode(unsigned char *buf, int len, void **req);
bool send_end_complete_rsp_encode(void *rsp, unsigned char **buf, int *len);
bool send_end_complete_rsp_decode(unsigned char *buf, int len, void **rsp);

struct serialize_handle{
	int packet_type;
	bool (*serialize)(void *obj, unsigned char **buf, int *len);
	bool (*deserialize)(unsigned char *buf, int len, void **obj);
};

#endif
