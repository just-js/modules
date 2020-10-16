#ifndef JUST_TCC_H
#define JUST_TCC_H

#include "just.h"
#include <libtcc.h>

namespace just {

namespace tcc {

void Compile(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}
#ifdef SHARED
extern "C" {
	void* _register_tcc() {
		return (void*)just::tcc::Init;
	}
}
#endif
#endif
