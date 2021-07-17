#!/bin/bash
g++ -c -fPIC -std=c++17 -Ideps/openssl-OpenSSL_1_1_1d/include -flto sign.cc
g++ -pthread deps/openssl-OpenSSL_1_1_1d/libcrypto.a sign.o -lcrypto -o sign

