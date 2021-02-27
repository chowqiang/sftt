#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

char *validate_req_encode(void *req, int *len);
void *validate_req_decode(char *buf, int len);

char *validate_rsp_encode(void *req, int *len);
void *validate_rsp_decode(char *buf, int );

char *send_file_name_req_encode(void *req);
void *send_file_name_rsp_decode(char *buf); 

char *send_data_req_encode(void *req);
void *send_data_rsp_decode(char *buf);



struct {
	int packet_type;
	char *(*serialize)(void *obj);
	void *(*deserialize)(char *buf);
} serializables[] = {

};

#endif
