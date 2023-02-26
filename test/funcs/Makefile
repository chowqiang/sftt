
all: client_task get put fwd_get fwd_put update_port fwd_ll

CFLAGS += -DSERVER_PATH=\"$(SERVER_PATH)\" -DCLIENT_PATH=\"$(CLIENT_PATH)\"

client_task: client_task.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB) -lpthread

get: get.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

put: put.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

fwd_get: fwd_get.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

fwd_put: fwd_put.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

update_port: update_port.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

fwd_ll: fwd_ll.c
	$(CC) -o $O/$@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)
