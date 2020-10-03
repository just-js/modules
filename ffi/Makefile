C=gcc
CC=g++

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

ffi.so: ## build shared library
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ffi.cc
	$(CC) -shared -flto -pthread -m64 -Wl,--start-group ffi.o -Wl,--end-group -Wl,-soname=jffi.so -lffi -o ffi.so

ffi.a: ## build static library
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter ffi.cc
	ar -crsT ffi.a ffi.o

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make ffi.so

cleanall: ## tidy up dependencies
	make clean

install: ## copy shared libarary to /usr/local/lib/just
	sudo mkdir -p /usr/local/lib/just && sudo cp -f ffi.so /usr/local/lib/just/

.DEFAULT_GOAL := help
