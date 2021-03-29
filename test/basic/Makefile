
all: dlist mem_pool

dlist: dlist.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

mem_pool: mem_pool.c
	$(CC) -o $O/$@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)
