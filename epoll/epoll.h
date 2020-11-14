#ifndef JUST_LOOP_H
#define JUST_LOOP_H

#include <just.h>
#include <sys/epoll.h>

namespace just {

namespace epoll {

void EpollCtl(const FunctionCallbackInfo<Value> &args);
void EpollCreate(const FunctionCallbackInfo<Value> &args);
void EpollWait(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_epoll() {
		return (void*)just::epoll::Init;
	}
}

#endif
