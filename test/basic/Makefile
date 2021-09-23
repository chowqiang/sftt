
all: dlist mem_pool net_trans serialize utils db md5 file strops list \
	client_task resp pthread

dlist: dlist.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

mem_pool: mem_pool.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

net_trans: net_trans.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

serialize: serialize.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

utils: utils.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

db: db.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

md5: md5.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

file: file.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

strops: strops.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

list: list.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

resp: resp.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

client_task: client_task.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB) -lpthread

pthread: pthread.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB) -lpthread
