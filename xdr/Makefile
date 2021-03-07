all: xdr.h xdr.c

xdr.h: xdr.x
	rm -f $(ROOT_DIR)/head/$@
	rpcgen -h $^ -o $(ROOT_DIR)/head/$@

xdr.c: xdr.x
	rm -f $(ROOT_DIR)/src/$@
	rpcgen -c $^ -o $(ROOT_DIR)/src/$@
