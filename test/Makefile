BASIC_DIR=$(TEST_DIR)/basic
FUNCS_DIR=$(TEST_DIR)/funcs

export LIB_DIR LIB CFLAGS TEST_DIR BASIC_DIR LIB_NAME

all: BASIC FUNCS

BASIC:
	$(shell if [ ! -d bin ]; then mkdir bin; fi)
	make -C $(BASIC_DIR) O=../bin

FUNCS:
	$(shell if [ ! -d bin ]; then mkdir bin; fi)
	make -C $(FUNCS_DIR) O=../bin

.PHONY: help
help:
	@echo "Running tests:"
	@echo ""
	@echo " test              - build and run function tests"

.PHONY: clean

clean:
	@rm -f $(TEST_DIR)/bin/*
	@rm -f $(BASIC_DIR)/*.o
