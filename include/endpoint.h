#ifndef _END_POINT_H_
#define _END_POINT_H_

#define UPDATE_THRESHOLD	100

int get_random_port();

int make_client(char *host, int port);

int make_server(int port);
#endif
