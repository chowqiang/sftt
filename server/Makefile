CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(CUR_OBJS) $(SERVER_BIN)

$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAG) -D PROC_NAME='"$(SERVER_BIN)"'

$(SERVER_BIN):$(CUR_OBJS)
	$(CC) -o $@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)
