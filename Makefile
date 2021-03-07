CC=gcc
ROOT_DIR=$(shell pwd)
CFLAG=-g -I$(ROOT_DIR)/head -lpthread -lcurses -DDEBUG_ENABLE
OBJS_DIR=$(ROOT_DIR)/src
OBJS=src
SERVER_DIR=$(ROOT_DIR)/server
CLIENT_DIR=$(ROOT_DIR)/client
SERVER_BIN=sfttd
CLIENT_BIN=sftt
XDR=xdr

export CC ROOT_DIR CFLAG SERVER_BIN CLIENT_BIN OBJS_DIR

all: $(XDR) $(SERVER_BIN) $(CLIENT_BIN) $(OBJS)

$(XDR): ECHO
	make -C $@

$(OBJS): ECHO
	make -C $@

$(SERVER_BIN): $(OBJS) ECHO
	make -C $(SERVER_DIR)

$(CLIENT_BIN): $(OBJS) ECHO
	make -C $(CLIENT_DIR)

ECHO:
	@echo $(OBJS) $(SERVER_BIN) $(CLIENT_BIN)


.PHONY: clean

clean:
	@rm $(ROOT_DIR)/src/*.o
	@rm $(ROOT_DIR)/client/*.o
	@rm $(ROOT_DIR)/client/$(CLIENT_BIN)
	@rm $(ROOT_DIR)/server/*.o
	@rm $(ROOT_DIR)/server/$(SERVER_BIN)
