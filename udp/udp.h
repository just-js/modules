#ifndef JUST_UDP_H
#define JUST_UDP_H

#include <just.h>
#include <arpa/inet.h>

namespace just {

namespace udp {
void RecvMsg(const FunctionCallbackInfo<Value> &args);
void SendMsg(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_udp() {
		return (void*)just::udp::Init;
	}
}

#endif
