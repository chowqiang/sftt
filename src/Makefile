CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(CUR_OBJS)

$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAG)
