all: mp_stat

mp_stat: mp_stat.c
	$(CC) -o $@ $^ $(CFLAG) -L$(LIB_DIR) -l$(LIB)

.PHONY: clean

clean:
	@rm -f $(TOOLS_DIR)/mp_stat
