/*
 * Automatically generated - do not edit
 */

#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include <stdbool.h>

bool validate_req_encode(void *req, unsigned char **buf, int *len);

bool validate_req_decode(unsigned char *buf, int len, void **req);

bool validate_resp_encode(void *req, unsigned char **buf, int *len);

bool validate_resp_decode(unsigned char *buf, int len, void **req);

bool pwd_req_encode(void *req, unsigned char **buf, int *len);

bool pwd_req_decode(unsigned char *buf, int len, void **req);

bool pwd_resp_encode(void *req, unsigned char **buf, int *len);

bool pwd_resp_decode(unsigned char *buf, int len, void **req);

#endif
