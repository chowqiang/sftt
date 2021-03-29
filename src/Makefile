CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(CUR_OBJS) $(LIB_NAME) 

$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ $(CFLAG)

$(LIB_NAME): $(CUR_OBJS)
	ar cr $(LIB_DIR)/$(LIB_NAME) $(CUR_OBJS)
