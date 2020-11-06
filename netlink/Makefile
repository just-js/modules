C=gcc
CC=g++
MODULE=netlink
LIBS=/usr/local/lib/just

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

shared: ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so

debug-shared: ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so

static: ## build static library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	ar -crsT ${MODULE}.a ${MODULE}.o

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make shared
	make static

cleanall: ## tidy up dependencies
	make clean

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS} && cp -f ${MODULE}.so ${LIBS}

.DEFAULT_GOAL := help
