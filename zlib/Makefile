C=gcc
CC=g++
ZLIB_VERSION="1.2.11"

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/zlib-$(ZLIB_VERSION).tar.gz https://github.com/madler/zlib/archive/v$(ZLIB_VERSION).tar.gz
	tar -zxvf deps/zlib-$(ZLIB_VERSION).tar.gz	-C deps/
	make -C deps/zlib-$(ZLIB_VERSION)/ distclean
	cd deps/zlib-$(ZLIB_VERSION) && CFLAGS='-mstackrealign -fPIC -O3' ./configure --static && cd ../../
	make -C deps/zlib-$(ZLIB_VERSION)/

zlib.so: ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/zlib-$(ZLIB_VERSION) -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter zlib.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group zlib.o deps/zlib-$(ZLIB_VERSION)/libz.a -Wl,--end-group -Wl,-soname=zlib.so -o zlib.so

zlib.a: ## build static library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/zlib-$(ZLIB_VERSION) -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter zlib.cc
	gcc-ar -crsT zlib.a zlib.o deps/zlib-$(ZLIB_VERSION)/libz.a

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

install: ## copy shared libarary to /usr/local/lib/just
	sudo mkdir -p /usr/local/lib/just && sudo cp -f zlib.so /usr/local/lib/just/

.DEFAULT_GOAL := help
