C=gcc
CC=g++
BLAKE3_VERSION="0.3.6"
LIBS=/usr/local/lib/just
MODULE=blake3

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/blake3-$(BLAKE3_VERSION).tar.gz https://github.com/BLAKE3-team/BLAKE3/archive/$(BLAKE3_VERSION).tar.gz
	tar -zxvf deps/blake3-$(BLAKE3_VERSION).tar.gz	-C deps/

libs: deps ## compile the object files 
	$(C) -c -fPIC -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3.c
	$(C) -c -fPIC -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_dispatch.c
	$(C) -c -fPIC -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_portable.c
	$(C) -c -fPIC -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_sse41_x86-64_unix.S
	$(C) -c -fPIC -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_avx2_x86-64_unix.S
	$(C) -c -fPIC -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_avx512_x86-64_unix.S
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter -o blake3_lib.o ${MODULE}.cc

library: ## build shared library
	make libs
	$(CC) -g -s -shared -flto -pthread -m64 -Wl,--start-group blake3_lib.o blake3.o blake3_dispatch.o blake3_portable.o blake3_avx2_x86-64_unix.o blake3_avx512_x86-64_unix.o blake3_sse41_x86-64_unix.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
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
