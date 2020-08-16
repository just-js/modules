#ifndef JUST_FFI_H
#define JUST_FFI_H

#include <just.h>
#include <ffi.h>

namespace just {

namespace ffi {

void FfiPrepCif(const FunctionCallbackInfo<Value> &args);
void FfiPrepCifVar(const FunctionCallbackInfo<Value> &args);
void FfiCall(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}
#ifdef SHARED
extern "C" {
	void* _register_ffi() {
		return (void*)just::ffi::Init;
	}
}
#endif
#endif
