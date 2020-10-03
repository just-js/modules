#!/bin/bash
make -C blake3/ clean blake3.so install
make -C ffi/ clean ffi.so install
make -C html/ clean html.so install
make -C openssl/ clean openssl.so install
make -C pg/ clean pg.so install
make -C picohttp/ clean http.so install
make -C rocksdb/ clean rocksdb.so install
make -C zlib/ clean zlib.so install