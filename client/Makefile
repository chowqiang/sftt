OBJS=$(OBJS_DIR)/*.o

VERSION=0.0.2
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(CUR_OBJS) $(CLIENT_BIN)

$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAG) -D PROC_NAME='"$(CLIENT_BIN)"' -D VERSION='"$(VERSION)"'

$(CLIENT_BIN):$(CUR_OBJS) $(OBJS)
	$(CC) -o $@ $^ $(CFLAG)
