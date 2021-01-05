#ifndef JUST_FIB_H
#define JUST_FIB_H

#include <just.h>

namespace just {

namespace wasm {

void Fibonacci(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_wasm() {
		return (void*)just::wasm::Init;
	}
}

#endif
