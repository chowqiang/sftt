OBJS=$(OBJS_DIR)/*.o

CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
C_CFLAG=$(CFLAG)

all:$(CUR_OBJS) $(CLIENT_BIN)

$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(C_CFLAG)

$(CLIENT_BIN):$(CUR_OBJS) $(OBJS)
	$(CC) -o $@ $^ $(C_CFLAG)
