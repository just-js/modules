#ifndef JUST_NETLINK_H
#define JUST_NETLINK_H

#include <just.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

namespace just {

namespace netlink {

void Fibonacci(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_netlink() {
		return (void*)just::netlink::Init;
	}
}

#endif
