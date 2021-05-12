
all: dlist mem_pool net_trans serialize utils

dlist: dlist.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

mem_pool: mem_pool.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

net_trans: net_trans.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

serialize: serialize.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

utils: utils.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)
