C=gcc
CC=g++
MODULE=jkvm
LIBS=/usr/local/lib/just
FLAGS=${CFLAGS}
LFLAG=${LFLAGS}

ARCH=x86_64
STDLIBS=deps/kvmtool-master/devices.o deps/kvmtool-master/disk/core.o deps/kvmtool-master/guest_compat.o deps/kvmtool-master/hw/rtc.o deps/kvmtool-master/hw/serial.o deps/kvmtool-master/ioport.o deps/kvmtool-master/irq.o deps/kvmtool-master/kvm-cpu.o deps/kvmtool-master/kvm.o deps/kvmtool-master/mmio.o deps/kvmtool-master/pci.o deps/kvmtool-master/term.o deps/kvmtool-master/vfio/core.o deps/kvmtool-master/vfio/pci.o deps/kvmtool-master/virtio/blk.o deps/kvmtool-master/virtio/scsi.o deps/kvmtool-master/virtio/console.o deps/kvmtool-master/virtio/core.o deps/kvmtool-master/virtio/net.o deps/kvmtool-master/virtio/rng.o deps/kvmtool-master/virtio/balloon.o deps/kvmtool-master/virtio/pci.o deps/kvmtool-master/disk/blk.o deps/kvmtool-master/disk/qcow.o deps/kvmtool-master/disk/raw.o deps/kvmtool-master/ioeventfd.o deps/kvmtool-master/net/uip/core.o deps/kvmtool-master/net/uip/arp.o deps/kvmtool-master/net/uip/icmp.o deps/kvmtool-master/net/uip/ipv4.o deps/kvmtool-master/net/uip/tcp.o deps/kvmtool-master/net/uip/udp.o deps/kvmtool-master/net/uip/buf.o deps/kvmtool-master/net/uip/csum.o deps/kvmtool-master/net/uip/dhcp.o deps/kvmtool-master/util/init.o deps/kvmtool-master/util/iovec.o deps/kvmtool-master/util/rbtree.o deps/kvmtool-master/util/threadpool.o deps/kvmtool-master/util/parse-options.o deps/kvmtool-master/util/rbtree-interval.o deps/kvmtool-master/util/strbuf.o deps/kvmtool-master/util/read-write.o deps/kvmtool-master/util/util.o deps/kvmtool-master/virtio/9p-pdu.o deps/kvmtool-master/kvm-ipc.o deps/kvmtool-master/virtio/mmio.o
X86LIBS=deps/kvmtool-master/x86/bios.o deps/kvmtool-master/x86/boot.o deps/kvmtool-master/x86/cpuid.o deps/kvmtool-master/x86/interrupt.o deps/kvmtool-master/x86/ioport.o deps/kvmtool-master/x86/irq.o deps/kvmtool-master/x86/kvm-cpu.o deps/kvmtool-master/x86/kvm.o deps/kvmtool-master/x86/mptable.o
BIOSLIBS=deps/kvmtool-master/x86/bios/bios-rom.o

.PHONY: help clean

help:
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_\.-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

deps: 
	mkdir -p deps
	curl -L -o deps/kvmtool-master.tar.gz https://codeload.github.com/billywhizz/kvmtool/tar.gz/master
	tar -zxvf deps/kvmtool-master.tar.gz -C deps/
	make -C deps/kvmtool-master/ lkvm
	rm deps/kvmtool-master.tar.gz

lkvm.a:
	cd deps/kvmtool-master
	ar crsT lkvm.a ${STDLIBS} ${X86LIBS} ${BIOSLIBS}
	cd ..

library: ## build shared library
	make -C deps/kvmtool-master/ lkvm
	$(CC) ${FLAGS} -c -fno-exceptions -ffunction-sections -fdata-sections -fPIC -std=c++17 -DV8_COMPRESS_POINTERS -Ideps/kvmtool-master/include -Ideps/kvmtool-master/x86/include -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -I$(JUST_HOME) -I$(JUST_HOME)/deps/v8/include -DBUILD_ARCH='${ARCH}' -g -O3 -march=native -mtune=native -Wall -Wextra -flto -fno-strict-aliasing -fpermissive -Wno-unused-parameter ${MODULE}.cc
	$(CC) -g -shared -flto -pthread -m64 -Wl,--start-group ${STDLIBS} ${X86LIBS} ${BIOSLIBS} ${MODULE}.o -Wl,--end-group -Wl,-soname=${MODULE}.so -Wl,--gc-sections ${LFLAG} -o ${MODULE}.so -lpthread -lz -lrt -lutil
	objcopy --only-keep-debug ${MODULE}.so ${MODULE}.so.debug
	strip --strip-debug --strip-unneeded ${MODULE}.so

clean: ## tidy up
	rm -f *.o
	rm -f *.so
	rm -f *.so.debug

all: ## make all
	make clean
	make library

cleanall: ## tidy up dependencies
	make clean

install: ## copy shared library to $INSTALL
	mkdir -p ${LIBS}
	cp -f ${MODULE}.so ${LIBS}/${MODULE}.so

install-debug: ## copy debug symbols to $INSTALL
	mkdir -p ${LIBS}/.debug
	cp -f ${MODULE}.so.debug ${LIBS}/.debug/${MODULE}.so.debug
	objcopy --add-gnu-debuglink=${LIBS}/${MODULE}.so ${LIBS}/.debug/${MODULE}.so.debug

.DEFAULT_GOAL := help
