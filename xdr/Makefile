all: xdr.h xdr.c

xdr.h: xdr.x
	rm -f $(ROOT_DIR)/include/$@
	rpcgen -C -h $^ -o $(ROOT_DIR)/include/$@

xdr.c: xdr.x
	rm -f $(ROOT_DIR)/src/$@
	rpcgen -C -c $^ -o $(ROOT_DIR)/src/$@
