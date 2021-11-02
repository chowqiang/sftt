BASIC_DIR=$(TEST_DIR)/basic

export LIB_DIR LIB CFLAGS TEST_DIR BASIC_DIR LIB_NAME

all: BASIC

BASIC:
	$(shell if [ ! -d bin ]; then mkdir bin; fi)
	make -C $(BASIC_DIR) O=../bin

.PHONY: clean

clean:
	@rm -f $(TEST_DIR)/bin/*
	@rm -f $(BASIC_DIR)/*.o
