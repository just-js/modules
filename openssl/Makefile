C = gcc
CC = g++
LIBS=/usr/local/lib/just
MODULE=openssl

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/OpenSSL_1_1_1d.tar.gz https://github.com/openssl/openssl/archive/OpenSSL_1_1_1d.tar.gz
	tar -zxvf deps/OpenSSL_1_1_1d.tar.gz -C deps/

deps/openssl-OpenSSL_1_1_1d/libcrypto.a: deps
	cd deps/openssl-OpenSSL_1_1_1d &&	CFLAGS='-fPIC' ./config --release --static -static no-autoload-config no-autoerrinit no-autoalginit no-afalgeng no-deprecated no-capieng no-cms no-comp no-dgram no-dynamic-engine enable-ec_nistp_64_gcc_128 no-err no-filenames no-gost no-hw-padlock no-makedepend no-multiblock no-nextprotoneg no-pic no-rfc3779 no-shared no-srp no-srtp no-static-engine no-tests threads no-ui zlib no-ssl no-tls1 no-dtls no-aria no-bf no-blake2 no-camellia no-cast no-chacha no-cmac no-des no-idea no-mdc2 no-ocb no-poly1305 no-rc2 no-scrypt no-seed no-siphash no-sm3 no-sm4 no-whirlpool
	make -C deps/openssl-OpenSSL_1_1_1d/ clean
	make -C deps/openssl-OpenSSL_1_1_1d/ build_generated
	make -C deps/openssl-OpenSSL_1_1_1d/ libcrypto.a libssl.a

library: deps/openssl-OpenSSL_1_1_1d/libcrypto.a deps/openssl-OpenSSL_1_1_1d/libssl.a ## build shared library
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -Ideps/openssl-OpenSSL_1_1_1d/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter crypto.cc
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -Ideps/openssl-OpenSSL_1_1_1d/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter signing.cc
	$(CC) -c -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -I. -Ideps/openssl-OpenSSL_1_1_1d/include -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter tls.cc
	$(CC) -g -s -shared -flto -pthread -m64 -Wl,--start-group signing.o crypto.o tls.o deps/openssl-OpenSSL_1_1_1d/libcrypto.a deps/openssl-OpenSSL_1_1_1d/libssl.a -Wl,--end-group -Wl,-soname=${MODULE}.so -o ${MODULE}.so
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

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS}
	cp -f ${MODULE}.so ${LIBS}/${MODULE}.so

install-debug: ## copy debug symbols to $INSTALL
	mkdir -p ${LIBS}/.debug
	cp -f ${MODULE}.so.debug ${LIBS}/.debug/${MODULE}.so.debug
	objcopy --add-gnu-debuglink=${LIBS}/${MODULE}.so ${LIBS}/.debug/${MODULE}.so.debug

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

.DEFAULT_GOAL := help
