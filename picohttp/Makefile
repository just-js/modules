C = gcc
CC = g++

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: ## dependencies
	mkdir -p deps/picohttpparser
	curl -L -o deps/picohttpparser/picohttpparser.h https://raw.githubusercontent.com/h2o/picohttpparser/master/picohttpparser.h
	curl -L -o deps/picohttpparser/picohttpparser.c https://raw.githubusercontent.com/h2o/picohttpparser/master/picohttpparser.c

picohttpparser.o: ## compile picohttpparser lib
	$(C) -fPIC -c -DV8_COMPRESS_POINTERS -Ideps/picohttpparser -g -O3 -Wall -Wextra -march=native -mtune=native -msse4 deps/picohttpparser/picohttpparser.c

picohttpparser.debug: ## compile picohttpparser lib
	$(C) -fPIC -c -DV8_COMPRESS_POINTERS -Ideps/picohttpparser -g -O3 -Wall -Wextra -march=native -mtune=native -msse4 deps/picohttpparser/picohttpparser.c

http.so: ## build shared library
	make picohttpparser.o
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -Ideps/picohttpparser -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter http.cc
	$(CC) -s -shared -flto -pthread -m64 -Wl,--start-group http.o picohttpparser.o -Wl,--end-group -Wl,-soname=http.so -o http.so

debug: ## build shared library
	make picohttpparser.debug
	$(CC) -c -fPIC -std=c++11 -DV8_COMPRESS_POINTERS -I. -Ideps/picohttpparser -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -g -O3 -march=native -mtune=native -Wall -Wextra -flto -Wno-unused-parameter http.cc
	$(CC) -shared -flto -pthread -m64 -Wl,--start-group http.o picohttpparser.o -Wl,--end-group -Wl,-soname=http.so -o http.so

http.a: ## build static library
	make picohttpparser.o
	$(CC) -c -std=c++11 -DV8_COMPRESS_POINTERS -I. -Ideps/picohttpparser -I../../just -I../../just/deps/v8/include -O3 -march=native -mtune=native -Wall -Wextra -Wno-unused-parameter http.cc
	ar -crs http.a picohttpparser.o http.o

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.a

all: ## make all
	make clean
	make deps
	make http.so
	make http.a

install: ## copy shared libarary to /usr/local/lib/just
	sudo mkdir -p /usr/local/lib/just && sudo cp -f http.so /usr/local/lib/just/

cleanall: ## tidy up dependencies
	make clean
	rm -fr deps

.DEFAULT_GOAL := help
