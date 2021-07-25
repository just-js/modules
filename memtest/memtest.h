#ifndef JUST_MEMTEST_H
#define JUST_MEMTEST_H

#include <just.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdarg.h>

namespace just {

namespace memtest {

void ReadOne(const FunctionCallbackInfo<Value> &args);
void ReadTwo(const FunctionCallbackInfo<Value> &args);

void* getBufferData (Local<Value> arg);

void ReadArrayBuffer(const FunctionCallbackInfo<Value> &args);
void ReadBufferObject(const FunctionCallbackInfo<Value> &args);
void ReadArrayBufferFast(const FunctionCallbackInfo<Value> &args);
void CreateBufferObject(const FunctionCallbackInfo<Value> &args);
void CreateArrayBuffer(const FunctionCallbackInfo<Value> &args);
void CreateArrayBufferInternal(const FunctionCallbackInfo<Value> &args);
void CreateExternalBuffer(const FunctionCallbackInfo<Value> &args);
void CreateExternalStaticBuffer(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_memtest() {
		return (void*)just::memtest::Init;
	}
}

#endif
