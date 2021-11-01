CC=gcc

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

CFLAGS=-g -Wall -Wno-unused-variable -I$(ROOT_DIR)/include -lpthread -lcurses -lsqlite3

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	CFLAGS += -ltirpc
endif

export CC ROOT_DIR CFLAGS OBJS_DIR LIB_NAME LIB LIB_DIR SERVER_BIN \
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
	cd test/scripts && ./gettest file

puttest: $(TEST)
	cd test/scripts && ./puttest file

mps: $(TEST)
	cd test/scripts && ./mps

basictest: $(TEST)
	cd test && ./Run testcases

ECHO:
	@echo $(LIB_NAME) $(SERVER_BIN) $(CLIENT_BIN) $(TEST) $(XDR)

.PHONY: clean

clean:
	make clean -C $(OBJS_DIR)
	make clean -C $(SERVER_DIR)
	make clean -C $(CLIENT_DIR)
	make clean -C $(TEST_DIR)
	make clean -C $(TOOLS_DIR)
