#ifndef JUST_FIB_H
#define JUST_FIB_H

#include <just.h>

namespace just {

namespace fib {

void Fibonacci(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_fib() {
		return (void*)just::fib::Init;
	}
}

#endif
