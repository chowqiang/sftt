#ifndef _SERIALIZE_HANDLE_H_
#define _SERIALIZE_HANDLE_H_

struct serialize_handler {
	int packet_type;
	bool (*serialize)(void *obj, unsigned char **buf, int *len);
	bool (*deserialize)(unsigned char *buf, int len, void **obj);
};

#endif
