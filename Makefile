CC=gcc
ROOT_DIR=$(shell pwd)
CFLAG=-I$(ROOT_DIR)/head -lpthread -DDEBUG_ENABLE
OBJS_DIR=$(ROOT_DIR)/src
OBJS=src
SERVER_BIN=server
CLIENT_BIN=client

export CC ROOT_DIR CFLAG SERVER_BIN CLIENT_BIN OBJS_DIR

all:$(SERVER_BIN) $(CLIENT_BIN) $(OBJS)

$(OBJS):ECHO
	make -C $@

$(SERVER_BIN):$(OBJS) ECHO
	make -C $@

$(CLIENT_BIN):$(OBJS) ECHO
	make -C $@

ECHO:
	@echo $(OBJS) $(SERVER_BIN) $(CLIENT_BIN)


.PHONY: clean

clean:
	@rm $(ROOT_DIR)/src/*.o
	@rm $(ROOT_DIR)/client/*.o
	@rm $(ROOT_DIR)/client/client
	@rm $(ROOT_DIR)/server/*.o
	@rm $(ROOT_DIR)/server/server
