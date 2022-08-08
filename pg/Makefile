C = gcc
CC = g++
LIBS=/usr/local/lib/just
MODULE=pg

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -o deps/postgresql-12.3.tar.gz https://ftp.postgresql.org/pub/source/v12.3/postgresql-12.3.tar.gz
	tar -zxvf deps/postgresql-12.3.tar.gz -C deps/
	unset MAKELEVEL && unset MAKEFLAGS && unset MFLAGS && cd deps/postgresql-12.3 && CFLAGS="-fPIC -Isrc/backend" ./configure --without-readline --without-zlib && make -C src/interfaces

library: ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/postgresql-12.3/src/include -Ideps/postgresql-12.3/src -Ideps/postgresql-12.3/src/interfaces/libpq -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -s -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o deps/postgresql-12.3/src/interfaces/libpq/libpq.a deps/postgresql-12.3/src/port/libpgport_shlib.a deps/postgresql-12.3/src/common/libpgcommon_shlib.a -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
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
