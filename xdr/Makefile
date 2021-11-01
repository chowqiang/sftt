all: gen_serialize req_resp.h req_resp.c gen_trans

ROOT_DIR = ..
req_resp.h: req_resp.x
	rm -f $(ROOT_DIR)/include/$@
	rpcgen -C -h $^ -o $(ROOT_DIR)/include/$@

req_resp.c: req_resp.x
	rm -f $(ROOT_DIR)/src/$@
	rpcgen -C -c $^ -o $(ROOT_DIR)/src/$@

gen_serialize: gen_serialize.c req_resp.x
	gcc -o $@ $<
	./$@ req_resp.x $(ROOT_DIR)/include/serialize.h $(ROOT_DIR)/src/serialize.c

gen_trans: gen_trans.c req_resp.x
	gcc -o $@ $<
	./$@ req_resp.x $(ROOT_DIR)/include/trans.h $(ROOT_DIR)/src/trans.c
