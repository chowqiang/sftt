
all: dlist mem_pool net_trans serialize utils db md5 file strops

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

db: db.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

md5: md5.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

file: file.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

strops: strops.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)
