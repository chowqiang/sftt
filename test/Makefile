BASIC_DIR=$(TEST_DIR)/basic

export LIB_DIR LIB CFLAG TEST_DIR BASIC_DIR LIB_NAME

all: BASIC
 
BASIC:
	make -C $(BASIC_DIR) O=../bin

.PHONY: clean

clean:
	@rm -f $(TEST_DIR)/bin/*
	@rm -f $(BASIC_DIR)/*.o
