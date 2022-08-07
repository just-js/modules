C = gcc
CC = g++
LIBS=/usr/local/lib/just
MODULE=bestline

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: 
	mkdir -p deps
	curl -L -o deps/bestline.tar.gz https://codeload.github.com/jart/bestline/tar.gz/master
	tar -zxvf deps/bestline.tar.gz -C deps/

deps/bestline-master/bestline.o: deps ## dependencies
	cd deps/bestline-master
	$(C) -fPIC -c -O3 -o bestline.o bestline.c
	cd ../../

library: ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/bestline-master -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -s -shared -flto -pthread -m64 -Wl,--start-group deps/bestline-master/bestline.o ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so -ldl -lrt
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.so.debug

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
