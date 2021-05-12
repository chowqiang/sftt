CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(CUR_OBJS) $(CLIENT_BIN)

$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAG)

$(CLIENT_BIN):$(CUR_OBJS)
	$(CC) -o $@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)
