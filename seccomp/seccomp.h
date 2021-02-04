#ifndef JUST_ZLIB_H
#define JUST_ZLIB_H

#include "just.h"
#include <seccomp.h>

namespace just {

namespace seccomp {

#define SYSCALL_NAME_MAX_LEN 128

void Create(const FunctionCallbackInfo<Value> &args);
void AddRule(const FunctionCallbackInfo<Value> &args);
void Load(const FunctionCallbackInfo<Value> &args);
void Release(const FunctionCallbackInfo<Value> &args);
void GetName(const FunctionCallbackInfo<Value> &args);
void GetNumber(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_seccomp() {
		return (void*)just::seccomp::Init;
	}
}

#endif
