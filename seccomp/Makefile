C=gcc
CC=g++
SECCOMP_VERSION=2.5.1
LIBS=/usr/local/lib/just
MODULE=seccomp

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/libseccomp-${SECCOMP_VERSION}.tar.gz https://github.com/seccomp/libseccomp/releases/download/v${SECCOMP_VERSION}/libseccomp-${SECCOMP_VERSION}.tar.gz
	tar -zxvf deps/libseccomp-${SECCOMP_VERSION}.tar.gz -C deps/
	#make -C deps/libseccomp-${SECCOMP_VERSION}/ clean
	cd deps/libseccomp-${SECCOMP_VERSION} && CFLAGS='-mstackrealign -fPIC -O3' ./configure --enable-shared=no && cd ../../
	make -C deps/libseccomp-${SECCOMP_VERSION}/

library: deps ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/libseccomp-${SECCOMP_VERSION}/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o deps/libseccomp-${SECCOMP_VERSION}/src/.libs/libseccomp.a -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so

library-debug: deps ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/libseccomp-${SECCOMP_VERSION}/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o deps/libseccomp-${SECCOMP_VERSION}/src/.libs/libseccomp.a -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so

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
	mkdir -p ${LIBS} && cp -f ${MODULE}.so ${LIBS}

.DEFAULT_GOAL := help
