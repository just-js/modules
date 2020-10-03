C=gcc
CC=g++
BLAKE3_VERSION="0.3.6"

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps
	curl -L -o deps/blake3-$(BLAKE3_VERSION).tar.gz https://github.com/BLAKE3-team/BLAKE3/archive/$(BLAKE3_VERSION).tar.gz
	tar -zxvf deps/blake3-$(BLAKE3_VERSION).tar.gz	-C deps/

libs: deps ## compile the object files 
	$(C) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3.c
	$(C) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_dispatch.c
	$(C) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_portable.c
	$(C) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_sse41_x86-64_unix.S
	$(C) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_avx2_x86-64_unix.S
	$(C) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter deps/BLAKE3-$(BLAKE3_VERSION)/c/blake3_avx512_x86-64_unix.S
	$(CC) -c -DSHARED -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -Ideps/BLAKE3-$(BLAKE3_VERSION)/c -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter -o blake3_lib.o blake3.cc

blake3.so: ## build shared library
	make libs
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group blake3_lib.o blake3.o blake3_dispatch.o blake3_portable.o blake3_avx2_x86-64_unix.o blake3_avx512_x86-64_unix.o blake3_sse41_x86-64_unix.o -Wl,--end-group -Wl,-soname=blake3.so -o blake3.so

blake3.a: ## build static library
	make libs
	ar -crsT blake3.a blake3_lib.o blake3.o blake3_dispatch.o blake3_portable.o blake3_avx2_x86-64_unix.o blake3_avx512_x86-64_unix.o blake3_sse41_x86-64_unix.o

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make deps
	make blake3.so

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

install: ## copy shared libarary to /usr/local/lib/just
	sudo mkdir -p /usr/local/lib/just && sudo cp -f blake3.so /usr/local/lib/just/

.DEFAULT_GOAL := help
