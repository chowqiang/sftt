CC=gcc
CFLAG=-g -I$(ROOT_DIR)/include -lpthread -lcurses -lsqlite3 -DDEBUG_ENABLE

ROOT_DIR=$(shell pwd)
OBJS_DIR=$(ROOT_DIR)/src
LIB_DIR=$(ROOT_DIR)/lib
SERVER_DIR=$(ROOT_DIR)/server
CLIENT_DIR=$(ROOT_DIR)/client
TEST_DIR=$(ROOT_DIR)/test
XDR_DIR=$(ROOT_DIR)/xdr

LIB=sftt
LIB_NAME=libsftt.a
SERVER_BIN=sfttd
CLIENT_BIN=sftt
TEST=test
XDR=xdr

export CC ROOT_DIR CFLAG OBJS_DIR LIB_NAME LIB LIB_DIR SERVER_BIN CLIENT_BIN TEST_DIR

all: $(XDR) $(SERVER_BIN) $(CLIENT_BIN) $(LIB_NAME) $(TEST)

$(XDR): ECHO
	make -C $(XDR_DIR)

$(LIB_NAME): $(XDR) ECHO
	make -C $(OBJS_DIR)

$(SERVER_BIN): $(LIB_NAME) ECHO
	make -C $(SERVER_DIR)

$(CLIENT_BIN): $(LIB_NAME) ECHO
	make -C $(CLIENT_DIR)

$(TEST): $(LIB_NAME) ECHO
	make -C $(TEST_DIR)

ECHO:
	@echo $(LIB_NAME) $(SERVER_BIN) $(CLIENT_BIN) $(TEST) $(XDR)

.PHONY: clean

clean:
	@rm -f $(ROOT_DIR)/src/*.o
	@rm -f $(ROOT_DIR)/lib/$(LIB_NAME)
	@rm -f $(ROOT_DIR)/client/$(CLIENT_BIN)
	@rm -f $(ROOT_DIR)/server/$(SERVER_BIN)
	make clean -C $(TEST_DIR)
