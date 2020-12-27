CC=gcc
version=0.0.2
release_number=2

all: test 

test: test.c 
	$(CC) -o $@ $^ -DDEBUG_PRINT -D VERSION='"$(version)"' -D RELEASE_NUMBER=$(release_number)

.PHONY: clean

clean:
	rm test 
