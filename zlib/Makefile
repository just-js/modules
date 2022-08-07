C=gcc
CC=g++
ZLIB_VERSION=1.2.11
LIBS=/usr/local/lib/just
MODULE=zlib

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/zlib-$(ZLIB_VERSION).tar.gz https://github.com/madler/zlib/archive/v$(ZLIB_VERSION).tar.gz
	tar -zxvf deps/zlib-$(ZLIB_VERSION).tar.gz	-C deps/
	make -C deps/zlib-$(ZLIB_VERSION)/ distclean
	cd deps/zlib-$(ZLIB_VERSION) && CFLAGS='-mstackrealign -fPIC -O3' ./configure --static && cd ../../
	make -C deps/zlib-$(ZLIB_VERSION)/

library: deps ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/zlib-$(ZLIB_VERSION) -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -s -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o deps/zlib-$(ZLIB_VERSION)/libz.a -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.so.debug
	rm -f *.a

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
