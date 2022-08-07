C = gcc
CC = g++
LIBS=/usr/local/lib/just
MODULE=webrtc
LIBDATACHANNEL_VERSION=0.17.8

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: 
	mkdir -p deps
	curl -L -o deps/libdatachannel-$(LIBDATACHANNEL_VERSION).tar.gz https://github.com/paullouisageneau/libdatachannel/archive/v$(LIBDATACHANNEL_VERSION).tar.gz
	tar -zxvf deps/libdatachannel-$(LIBDATACHANNEL_VERSION).tar.gz -C deps/
	git submodule update --init --recursive
	cd deps/libdatachannel-$(LIBDATACHANNEL_VERSION)
	cmake -DUSE_GNUTLS=0 -DUSE_NICE=0 -DNO_MEDIA=1 -DNO_WEBSOCKET=1
	make datachannel datachannel-static juice juice-static usrsctp
	cd ../../../

library: deps ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -Ideps/libdatachannel-$(LIBDATACHANNEL_VERSION)/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ${MODULE}.o deps/libdatachannel-$(LIBDATACHANNEL_VERSION)/libdatachannel-static.a deps/libdatachannel-$(LIBDATACHANNEL_VERSION)/deps/libjuice/libjuice-static.a deps/libdatachannel-$(LIBDATACHANNEL_VERSION)/deps/usrsctp/usrsctplib/libusrsctp.a -Wl,--end-group -Wl,-soname=${MODULE}.so -lssl -lcrypto -o ${MODULE}.so
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so
	objcopy --add-gnu-debuglink=${MODULE}.so.debug ${MODULE}.so

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

.DEFAULT_GOAL := help
