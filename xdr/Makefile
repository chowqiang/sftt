all: gen_serialize req_rsp.h req_rsp.c

ROOT_DIR = ..
req_rsp.h: req_rsp.x
	rm -f $(ROOT_DIR)/include/$@
	rpcgen -C -h $^ -o $(ROOT_DIR)/include/$@

req_rsp.c: req_rsp.x
	rm -f $(ROOT_DIR)/src/$@
	rpcgen -C -c $^ -o $(ROOT_DIR)/src/$@

gen_serialize: gen_serialize.c req_rsp.x
	gcc -o $@ $<
	./$@ req_rsp.x $(ROOT_DIR)/include/serialize.h $(ROOT_DIR)/src/serialize.c
