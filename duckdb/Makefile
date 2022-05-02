C = gcc
CC = g++
LIBS=/usr/local/lib/just
MODULE=duckdb

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: 
	mkdir -p deps
	curl -L -o deps/libduckdb.tar.gz https://github.com/duckdb/duckdb/releases/download/v0.3.2/libduckdb-src.zip
	unzip deps/libduckdb.tar.gz -d deps/

deps/duckdb.o: deps ## dependencies
	cd deps
	$(CC) -fPIC -c -O3 -o duckdb.o duckdb.cpp
	cd ..

library: ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -Ideps -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -s -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o deps/duckdb.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so -ldl -lrt
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so

clean: ## tidy up
	rm -f *.o
	rm -f *.so

all: ## make all
	make clean
	make deps
	make library

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS}
	cp -f ${MODULE}.so ${LIBS}/${MODULE}.so

install-debug: ## copy debug symbols to $INSTALL
	mkdir -p ${LIBS}/.debug
	cp -f ${MODULE}.so.debug ${LIBS}/.debug/${MODULE}.so.debug
	objcopy --add-gnu-debuglink=${LIBS}/${MODULE}.so ${LIBS}/.debug/${MODULE}.so.debug

.DEFAULT_GOAL := help
