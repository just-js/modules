#ifndef JUST_SIGNAL_H
#define JUST_SIGNAL_H

#include <just.h>
#include <signal.h>
#include <sys/signalfd.h>

namespace just {

namespace signal {
void SignalFD(const FunctionCallbackInfo<Value> &args);
void SigEmptySet(const FunctionCallbackInfo<Value> &args);
void SigProcMask(const FunctionCallbackInfo<Value> &args);
void SigAddSet(const FunctionCallbackInfo<Value> &args);
void Signal(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_signal() {
		return (void*)just::signal::Init;
	}
}

#endif
