BASIC_DIR=$(TEST_DIR)/basic
FUNCS_DIR=$(TEST_DIR)/funcs

export LIB_DIR LIB CFLAGS TEST_DIR BASIC_DIR LIB_NAME SERVER_PATH CLIENT_PATH

all: BASIC FUNCS

BASIC:
	$(shell if [ ! -d bin ]; then mkdir bin; fi)
	make -C $(BASIC_DIR) O=../bin

FUNCS:
	$(shell if [ ! -d bin ]; then mkdir bin; fi)
	make -C $(FUNCS_DIR) O=../bin


ifeq ("$(CASE)", "")
	@echo "run all test cases ..."
else ifneq ("$(CASE)", "NO")
	@echo "run test case '$(CASE)'"
	@./Run $(TEST_DIR)/testcases $(CASE)
else
	@echo "don't run any test cases"
endif

.PHONY: help
help:
	@echo "Running tests:"
	@echo " test        - build and run function tests"

.PHONY: clean
clean:
	@rm -f $(TEST_DIR)/bin/*
	@rm -f $(BASIC_DIR)/*.o
