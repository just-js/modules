C=gcc
CC=g++
MODULE=http
LIBS=/usr/local/lib/just
FLAGS=${CFLAGS}
LFLAG=${LFLAGS}

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps/picohttpparser
	curl -L -o deps/picohttpparser/picohttpparser.h https://raw.githubusercontent.com/h2o/picohttpparser/master/picohttpparser.h
	curl -L -o deps/picohttpparser/picohttpparser.c https://raw.githubusercontent.com/h2o/picohttpparser/master/picohttpparser.c

picohttpparser.o: deps ## compile picohttpparser lib
	$(C) -fPIC -c -DV8_COMPRESS_POINTERS -Ideps/picohttpparser -g -O3 -Wall -Wextra -march=native -mtune=native -msse4 deps/picohttpparser/picohttpparser.c

library: ## build shared library
	make picohttpparser.o
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -Ideps/picohttpparser -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o picohttpparser.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
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

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS}
	cp -f ${MODULE}.so ${LIBS}/${MODULE}.so

install-debug: ## copy debug symbols to $INSTALL
	mkdir -p ${LIBS}/.debug
	cp -f ${MODULE}.so.debug ${LIBS}/.debug/${MODULE}.so.debug
	objcopy --add-gnu-debuglink=${LIBS}/${MODULE}.so ${LIBS}/.debug/${MODULE}.so.debug

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

.DEFAULT_GOAL := help
