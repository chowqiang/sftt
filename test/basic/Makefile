
all: dlist mem_pool net_trans serialize utils db md5 file strops list \
	resp pthread options readline compress debug thread_pool priority_list \
	progress_viewer

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

pthread: pthread.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB) -lpthread

options: options.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

readline: readline.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB) -lreadline

compress: compress.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

debug: debug.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

thread_pool: thread_pool.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

priority_list: priority_list.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

progress_viewer: progress_viewer.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)
