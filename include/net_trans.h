#ifndef _NET_TRANS_H_
#define _NET_TRANS_H_

#include "packet.h"

sftt_packet *malloc_sftt_packet(int block_size);

int send_sftt_packet(int sock, sftt_packet *sp);

int recv_sftt_packet(int sock, sftt_packet *sp); 

void free_sftt_packet(sftt_packet **sp);

#endif
