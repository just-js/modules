C=gcc
CC=g++
LIBS=/usr/local/lib/just
MODULE=wasm
WABT_VERSION=1.0.20

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/wabt-$(WABT_VERSION).tar.gz https://github.com/WebAssembly/wabt/archive/$(WABT_VERSION).tar.gz
	tar -zxvf deps/wabt-$(WABT_VERSION).tar.gz	-C deps/

library: ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so

library-debug: ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so

clean: ## tidy up
	rm -f *.o
	rm -f *.so

all: ## make all
	make clean
	make library

cleanall: ## tidy up dependencies
	make clean

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS} && cp -f ${MODULE}.so ${LIBS}

.DEFAULT_GOAL := help
