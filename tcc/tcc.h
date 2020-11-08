#ifndef JUST_TCC_H
#define JUST_TCC_H

#include "just.h"
#include <libtcc.h>

namespace just {

namespace tcc {

void Compile(const FunctionCallbackInfo<Value> &args);
void GetSymbol(const FunctionCallbackInfo<Value> &args);
void AddSymbol(const FunctionCallbackInfo<Value> &args);
void Destroy(const FunctionCallbackInfo<Value> &args);
void Relocate(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_tcc() {
		return (void*)just::tcc::Init;
	}
}

#endif
