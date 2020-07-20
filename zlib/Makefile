C = gcc
CC = g++

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/zlib-1.2.11.tar.gz https://github.com/madler/zlib/archive/v1.2.11.tar.gz
	tar -zxvf deps/zlib-1.2.11.tar.gz	-C deps/
	make -C deps/zlib-1.2.11/ distclean
	cd deps/zlib-1.2.11 && CFLAGS='-mstackrealign -fPIC -O3' ./configure --static && cd ../../
	make -C deps/zlib-1.2.11/

zlib.so: ## build shared library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/zlib-1.2.11 -I../../just -I../../just/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter zlib.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group zlib.o deps/zlib-1.2.11/libz.a -Wl,--end-group -Wl,-soname=zlib.so -o zlib.so

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make deps
	make zlib.so

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

.DEFAULT_GOAL := help
