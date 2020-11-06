C=gcc
CC=g++
LIBS=/usr/local/lib/just

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

shared: ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ffi.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group ffi.o -Wl,--end-group -Wl,-soname=ffi.so -lffi -o ffi.so

debug-shared: ## build debug shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ffi.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ffi.o -Wl,--end-group -Wl,-soname=ffi.so -lffi -o ffi.so

static: ## build static library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ffi.cc
	ar -crsT ffi.a ffi.o

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

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS} && cp -f ffi.so ${LIBS}

.DEFAULT_GOAL := help
