#ifndef JUST_NET_H
#define JUST_NET_H

#include <just.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <linux/if_tun.h>

#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <errno.h>
#include <linux/route.h>
#include <linux/ipv6_route.h>
#include <linux/sockios.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/select.h>

namespace just {

namespace ip {

#define INET_ADDRLEN 4
#define INET6_ADDRLEN 16

void TunTap(const FunctionCallbackInfo<Value> &args);
void SetAddress(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_ip() {
		return (void*)just::ip::Init;
	}
}

#endif
