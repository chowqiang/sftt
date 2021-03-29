#ifndef _CONNECT_H_
#define _CONNECT_H_

enum connect_result {
	CONN_RET_CONNECT_FAILED,
	CONN_RET_VALIDATE_FAILED,
	CONN_RET_SERVER_BUSYING,
	CONN_RET_CONNECT_SUCCESS,
};

enum connect_type {
	CONN_TYPE_CTRL,
	CONN_TYPE_DATA
};

typedef struct {
	int sock;
	int block_size;
} sock_connect;

typedef struct {
	int sock;
	enum connect_type type;
	int port;
} client_sock_conn;

#endif
