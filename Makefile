CC=gcc
CFLAG=-g -I$(ROOT_DIR)/include -lpthread -lcurses -lsqlite3

ROOT_DIR=$(shell pwd)
OBJS_DIR=$(ROOT_DIR)/src
LIB_DIR=$(ROOT_DIR)/lib
SERVER_DIR=$(ROOT_DIR)/server
CLIENT_DIR=$(ROOT_DIR)/client
TOOLS_DIR=$(ROOT_DIR)/tools
TEST_DIR=$(ROOT_DIR)/test
XDR_DIR=$(ROOT_DIR)/xdr
CONFIG_DIR=$(ROOT_DIR)/config

LIB=sftt
LIB_NAME=libsftt.a
SERVER_BIN=sfttd
CLIENT_BIN=sftt
TOOLS=tools
TEST=test
XDR=xdr
CONFIG=config

export CC ROOT_DIR CFLAG OBJS_DIR LIB_NAME LIB LIB_DIR SERVER_BIN \
	CLIENT_BIN TOOLS_DIR TEST_DIR

all: $(CONFIG) $(XDR) $(SERVER_BIN) $(CLIENT_BIN) $(LIB_NAME) $(TEST) \
	$(TOOLS)

$(CONFIG): ECHO
	make -C $(CONFIG_DIR)

$(XDR): $(CONFIG) ECHO
	make -C $(XDR_DIR)

$(LIB_NAME): $(CONFIG) $(XDR) ECHO
	make -C $(OBJS_DIR)

$(SERVER_BIN): $(CONFIG) $(LIB_NAME) ECHO
	make -C $(SERVER_DIR)

$(CLIENT_BIN): $(CONFIG) $(LIB_NAME) ECHO
	make -C $(CLIENT_DIR)

$(TOOLS): $(LIB_NAME) ECHO
	make -C $(TOOLS_DIR)

$(TEST): $(LIB_NAME) ECHO
	make -C $(TEST_DIR)

gettest: $(TEST)
	cd test && ./scripts/gettest

puttest: $(TEST)
	cd test && ./scripts/puttest

basictest: $(TEST)
	cd test && ./Run testcases

ECHO:
	@echo $(LIB_NAME) $(SERVER_BIN) $(CLIENT_BIN) $(TEST) $(XDR)

.PHONY: clean

clean:
	@rm -f $(ROOT_DIR)/src/*.o
	@rm -f $(ROOT_DIR)/lib/$(LIB_NAME)
	@rm -f $(ROOT_DIR)/client/$(CLIENT_BIN)
	@rm -f $(ROOT_DIR)/server/$(SERVER_BIN)
	make clean -C $(TEST_DIR)
