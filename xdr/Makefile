all: req_rsp.h req_rsp.c

req_rsp.h: req_rsp.x
	rm -f $(ROOT_DIR)/include/$@
	rpcgen -C -h $^ -o $(ROOT_DIR)/include/$@

req_rsp.c: req_rsp.x
	rm -f $(ROOT_DIR)/src/$@
	rpcgen -C -c $^ -o $(ROOT_DIR)/src/$@
