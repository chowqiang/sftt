OBJS=$(OBJS_DIR)/*.o

CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(CUR_OBJS) $(SERVER_BIN)

$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAG) -D PROC_NAME='"$(SERVER_BIN)"'

$(SERVER_BIN):$(CUR_OBJS) $(OBJS)
	$(CC) -o $@ $^ $(CFLAG)
