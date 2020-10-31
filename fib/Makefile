C=gcc
CC=g++

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

shared: ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter fib.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group fib.o -Wl,--end-group -Wl,-soname=fib.so -o fib.so

debug-shared: ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter fib.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group fib.o -Wl,--end-group -Wl,-soname=fib.so -o fib.so

static: ## build static library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter fib.cc
	ar -crsT fib.a fib.o

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make shared
	make static

cleanall: ## tidy up dependencies
	make clean

install: ## copy shared library to /usr/local/lib/just
	sudo mkdir -p /usr/local/lib/just && sudo cp -f fib.so /usr/local/lib/just/

.DEFAULT_GOAL := help
