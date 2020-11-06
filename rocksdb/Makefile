C = gcc
CC = g++
LIBS=/usr/local/lib/just

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: 
	mkdir -p deps
	curl -L -o deps/rocksdb-6.10.2.tar.gz https://github.com/facebook/rocksdb/archive/v6.10.2.tar.gz
	tar -zxvf deps/rocksdb-6.10.2.tar.gz -C deps/

deps/rocksdb-6.10.2/librocksdb.a: deps ## dependencies
	make -C deps/rocksdb-6.10.2/ clean
	CFLAGS='-mstackrealign -fPIC -O3' make -C deps/rocksdb-6.10.2/ static_lib

shared: deps/rocksdb-6.10.2/librocksdb.a ## build shared library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -Ideps/rocksdb-6.10.2/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter rocksdb.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group rocksdb.o deps/rocksdb-6.10.2/librocksdb.a -Wl,--end-group -Wl,-soname=rocksdb.so -o rocksdb.so

debug-shared: deps/rocksdb-6.10.2/librocksdb.a ## build shared library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -Ideps/rocksdb-6.10.2/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter rocksdb.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group rocksdb.o deps/rocksdb-6.10.2/librocksdb.a -Wl,--end-group -Wl,-soname=rocksdb.so -o rocksdb.so

static: deps/rocksdb-6.10.2/librocksdb.a ## build static library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -Ideps/rocksdb-6.10.2/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter rocksdb.cc
	ar -crsT rocksdb.a rocksdb.o deps/rocksdb-6.10.2/librocksdb.a

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make deps
	make shared
	make static

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS} && cp -f rocksdb.so ${LIBS}

.DEFAULT_GOAL := help
