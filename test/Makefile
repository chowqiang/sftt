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
	@echo ""
	@echo "run all test cases ..."
else ifneq ("$(CASE)", "NO")
	@echo ""
	@echo "run test case '$(CASE)'"
	@./Run $(TEST_DIR)/testcases \"$(CASE)\"
else
	@echo "don't run any test cases"
endif

.PHONY: help
help:
	@echo "Running tests:"
	@echo ""
	@echo " test                      - build and run all tests"
	@echo " test CASE=<filter>        - filter the set of tests:"
	@echo "           by wildcard     - wildcard filter is <test_class>.<test_case>, each can be replaced by '*'"
	@echo "                             e.g. CASE='*' or CASE='*.*' is equivalent to run all tests"
	@echo "                                  CASE='basic' or CASE='basic.*' select all tests from basic test class"
	@echo "                                  CASE='*.get' selects a single test named get from all test class"
	@echo ""


.PHONY: clean
clean:
	@rm -f $(TEST_DIR)/bin/*
	@rm -f $(BASIC_DIR)/*.o
