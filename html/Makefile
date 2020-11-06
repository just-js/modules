C = gcc
CC = g++
LIBS=/usr/local/lib/just

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

shared: ## build shared library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter html.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group html.o -Wl,--end-group -Wl,-soname=html.so -o html.so

debug-shared: ## build debug shared library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter html.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group html.o -Wl,--end-group -Wl,-soname=html.so -o html.so

static: ## build static library
	$(CC) -c -std=c++11 -DV8_COMPRESS_POINTERS -I. -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -Wno-unused-parameter html.cc
	ar -crs html.a html.o

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make shared
	make static

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS} && cp -f html.so ${LIBS}

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

.DEFAULT_GOAL := help
