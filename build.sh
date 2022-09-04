#!/bin/bash
MODULES=${2:-"blake3 encode epoll ffi fs html http inspector io ip jkvm memory net openssl pg profiler rocksdb seccomp sha1 signal sqlite sys tcc thread udp vm zlib iouring webrtc rsync bestlines"}
for m in $MODULES
do
  make -C $m/ clean library
  sudo make -C $m/ install
  sudo make -C $m/ install-debug
done
