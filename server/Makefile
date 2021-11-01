CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(CUR_OBJS) $(SERVER_BIN)

$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAGS)

$(SERVER_BIN):$(CUR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -L$(LIB_DIR) -l$(LIB)

.PHONY: clean

clean:
	@rm -f $(CUR_OBJS)
