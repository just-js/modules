#!/bin/bash
## run this to compile all the modules and install them in /usr/local/lib/just
make -C blake3/ clean blake3.so install
make -C ffi/ clean ffi.so install
make -C html/ clean html.so install
make -C openssl/ clean openssl.so install
## pg currently broken - some Makefile weirdness
#make -C pg/ clean pg.so install
make -C picohttp/ clean http.so install
make -C rocksdb/ clean rocksdb.so install
make -C tinycc/ clean tcc.so install
make -C zlib/ clean zlib.so install
