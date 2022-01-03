
all: client_task progress_viewer get put fwd_get fwd_put

client_task: client_task.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB) -lpthread

progress_viewer: progress_viewer.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

compress: compress.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

get: get.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

put: put.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

fwd_get: fwd_get.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

fwd_put: fwd_put.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)
