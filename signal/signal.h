#ifndef JUST_SIGNAL_H
#define JUST_SIGNAL_H

#include <just.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <signal.h>
#include <unistd.h>
#include <sys/signalfd.h>
#include <inttypes.h>
#include <stdarg.h>

namespace just {

namespace signal {

struct signalHandler {
	v8::Persistent<Function, v8::NonCopyablePersistentTraits<Function>> callback;
  int signum;
	Isolate* isolate;
};
// todo - need to look at how to handle this across threads
// currently it is not thread safe and will only allow one signal handler across
// the process?
// https://stackoverflow.com/questions/15235526/the-static-keyword-and-its-various-uses-in-c
std::map<int, signalHandler*> handlers;

void SignalHandler(int signal, siginfo_t* info, void* void_context);

void SignalFD(const FunctionCallbackInfo<Value> &args);
void SigEmptySet(const FunctionCallbackInfo<Value> &args);
void SigProcMask(const FunctionCallbackInfo<Value> &args);
void SigFillSet(const FunctionCallbackInfo<Value> &args);
void SigAddSet(const FunctionCallbackInfo<Value> &args);
void SigWait(const FunctionCallbackInfo<Value> &args);
void SigAction(const FunctionCallbackInfo<Value> &args);
void Reset(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_signal() {
		return (void*)just::signal::Init;
	}
}

#endif
