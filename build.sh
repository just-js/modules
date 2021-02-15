#!/bin/bash
MODULES=${2:-"blake3 encode epoll ffi fs html http inspector memory net netlink openssl pg seccomp sha1 signal sys tcc thread udp vm zlib"}
for m in $MODULES
do
  make -C $m/ clean library
  sudo make -C $m/ install
done
