C = gcc
CC = g++
LIBS=/usr/local/lib/just
MODULE=sqlite

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: 
	mkdir -p deps
	curl -L -o deps/sqlite.tar.gz https://www.sqlite.org/src/tarball/sqlite.tar.gz?r=release
	tar -zxvf deps/sqlite.tar.gz -C deps/

deps/sqlite/build/.libs/libsqlite3.a: deps ## dependencies
	mkdir -p deps/sqlite/build
	cd deps/sqlite/build && ../configure --disable-math --disable-readline --disable-tcl --with-pic --enable-session && make

library: deps/sqlite/build/.libs/libsqlite3.a ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -Ideps/sqlite/build -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o deps/sqlite/build/.libs/libsqlite3.a -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so
	objcopy --add-gnu-debuglink=${MODULE}.so.debug ${MODULE}.so

vfs: deps/sqlite/build/.libs/libsqlite3.a ## build shared vfs library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -Ideps/sqlite/build -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}_vfs.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}_vfs.o deps/sqlite/build/.libs/libsqlite3.a -Wl,--end-group -Wl,-soname=${MODULE}_vfs.so -o ${MODULE}_vfs.so
	objcopy --only-keep-debug ${MODULE}_vfs.so ${MODULE}_vfs.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}_vfs.so
	objcopy --add-gnu-debuglink=${MODULE}.so.debug ${MODULE}.so

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
	cp -f ${MODULE}_vfs.so ${LIBS}/${MODULE}_vfs.so

install-debug: ## copy debug symbols to $INSTALL
	mkdir -p ${LIBS}/.debug
	cp -f ${MODULE}.so.debug ${LIBS}/.debug/${MODULE}.so.debug

.DEFAULT_GOAL := help
