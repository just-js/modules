#ifndef JUST_ZLIB_H
#define JUST_ZLIB_H

#include "just.h"
#include <blake3.h>

namespace just {

namespace blake3 {

void HashInit(const FunctionCallbackInfo<Value> &args);
void HashInitKeyed(const FunctionCallbackInfo<Value> &args);
void HashUpdate(const FunctionCallbackInfo<Value> &args);
void HashFinalize(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_blake3() {
		return (void*)just::blake3::Init;
	}
}

#endif
