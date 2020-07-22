C = gcc
CC = g++

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -o deps/postgresql-12.3.tar.gz https://ftp.postgresql.org/pub/source/v12.3/postgresql-12.3.tar.gz
	tar -zxvf deps/postgresql-12.3.tar.gz -C deps/
	cd deps/postgresql-12.3 && CFLAGS="-fPIC" ./configure --without-readline --without-zlib && cd ../../
	make -C deps/postgresql-12.3/src/interfaces/libpq

pg.so: ## build shared library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/postgresql-12.3/src/include -Ideps/postgresql-12.3/src -Ideps/postgresql-12.3/src/interfaces/libpq -I../../just -I../../just/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter pg.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group deps/postgresql-12.3/src/interfaces/libpq/libpq.a deps/postgresql-12.3/src/port/libpgport_shlib.a deps/postgresql-12.3/src/common/libpgcommon_shlib.a pg.o -Wl,--end-group -Wl,-soname=pg.so -o pg.so

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make deps
	make pg.so

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

.DEFAULT_GOAL := help
