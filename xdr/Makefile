all: gen_serialize gen_serialize_raw req_resp.h req_resp.c gen_trans

ROOT_DIR = ..
req_resp.h: req_resp.x
	rm -f $(ROOT_DIR)/include/$@
	rpcgen -C -h $^ -o $(ROOT_DIR)/include/$@

req_resp.c: req_resp.x
	rm -f $(ROOT_DIR)/src/$@
	rpcgen -C -c $^ -o $(ROOT_DIR)/src/$@
	sed -i $(ROOT_DIR)/src/$@ -e 's/return FALSE;/{printf("%s:%d\\n", __func__, __LINE__); return FALSE;}/g'
	sed -i $(ROOT_DIR)/src/$@ -e 's/#include "req_resp.h"/#include <stdio.h>\n#include "req_resp.h"/g'

gen_serialize: gen_serialize.c req_resp.x
	gcc -o $@ $<
	./$@ req_resp.x $(ROOT_DIR)/include/serialize.h $(ROOT_DIR)/src/serialize.c

gen_serialize_raw: gen_serialize_raw.c req_resp.x
	gcc -o $@ $<
	./$@ req_resp.x $(ROOT_DIR)/include/serialize_raw.h $(ROOT_DIR)/src/serialize_raw.c

gen_trans: gen_trans.c req_resp.x
	gcc -o $@ $<
	./$@ req_resp.x $(ROOT_DIR)/include/trans.h $(ROOT_DIR)/src/trans.c
