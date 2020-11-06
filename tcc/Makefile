C=gcc
CC=g++
TCC_VERSION=0.9.27
LIBS=/usr/local/lib/just

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/tcc-${TCC_VERSION}.tar.bz2 http://download.savannah.gnu.org/releases/tinycc/tcc-${TCC_VERSION}.tar.bz2
	tar -jxvf deps/tcc-${TCC_VERSION}.tar.bz2	-C deps/
	cd deps/tcc-${TCC_VERSION} && CFLAGS='-mstackrealign -fPIC -O3' ./configure && cd ../../
	make -C deps/tcc-${TCC_VERSION}/ libtcc.a

shared: deps ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/tcc-${TCC_VERSION} -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter tcc.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group tcc.o deps/tcc-${TCC_VERSION}/libtcc.a -Wl,--end-group -Wl,-soname=tcc.so -o tcc.so

debug-shared: deps ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/tcc-${TCC_VERSION} -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter tcc.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group tcc.o deps/tcc-${TCC_VERSION}/libtcc.a -Wl,--end-group -Wl,-soname=tcc.so -o tcc.so

static: deps ## build static library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/tcc-${TCC_VERSION} -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter tcc.cc
	gcc-ar -crsT tcc.a tcc.o deps/tcc-${TCC_VERSION}/libtcc.a

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make deps
	make shared
	make static

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS} && cp -f tcc.so ${LIBS}

.DEFAULT_GOAL := help
