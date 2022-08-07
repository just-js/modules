C = gcc
CC = g++
LIBS=/usr/local/lib/just
MODULE=rsync
DEPS=deps/librsync-2.3.2
OBJS=${DEPS}/CMakeFiles/rsync.dir/src/prototab.c.o ${DEPS}/CMakeFiles/rsync.dir/src/base64.c.o ${DEPS}/CMakeFiles/rsync.dir/src/buf.c.o ${DEPS}/CMakeFiles/rsync.dir/src/checksum.c.o ${DEPS}/CMakeFiles/rsync.dir/src/command.c.o ${DEPS}/CMakeFiles/rsync.dir/src/delta.c.o ${DEPS}/CMakeFiles/rsync.dir/src/emit.c.o ${DEPS}/CMakeFiles/rsync.dir/src/fileutil.c.o ${DEPS}/CMakeFiles/rsync.dir/src/hashtable.c.o ${DEPS}/CMakeFiles/rsync.dir/src/hex.c.o ${DEPS}/CMakeFiles/rsync.dir/src/job.c.o ${DEPS}/CMakeFiles/rsync.dir/src/mdfour.c.o ${DEPS}/CMakeFiles/rsync.dir/src/mksum.c.o ${DEPS}/CMakeFiles/rsync.dir/src/msg.c.o ${DEPS}/CMakeFiles/rsync.dir/src/netint.c.o ${DEPS}/CMakeFiles/rsync.dir/src/patch.c.o ${DEPS}/CMakeFiles/rsync.dir/src/readsums.c.o ${DEPS}/CMakeFiles/rsync.dir/src/rollsum.c.o ${DEPS}/CMakeFiles/rsync.dir/src/rabinkarp.c.o ${DEPS}/CMakeFiles/rsync.dir/src/scoop.c.o ${DEPS}/CMakeFiles/rsync.dir/src/stats.c.o ${DEPS}/CMakeFiles/rsync.dir/src/sumset.c.o ${DEPS}/CMakeFiles/rsync.dir/src/trace.c.o ${DEPS}/CMakeFiles/rsync.dir/src/tube.c.o ${DEPS}/CMakeFiles/rsync.dir/src/util.c.o ${DEPS}/CMakeFiles/rsync.dir/src/version.c.o ${DEPS}/CMakeFiles/rsync.dir/src/whole.c.o ${DEPS}/CMakeFiles/rsync.dir/src/blake2/blake2b-ref.c.o
.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: 
	mkdir -p deps
	curl -L -o deps/librsync.tar.gz https://codeload.github.com/librsync/librsync/tar.gz/v2.3.2
	tar -zxvf deps/librsync.tar.gz -C deps/
	cd ${DEPS}/
	cmake .
	make -j4
	cd ../../

library: ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/bestline-master -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -s -shared -flto -pthread -m64 -Wl,--start-group ${OBJS} ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so -ldl -lrt
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
