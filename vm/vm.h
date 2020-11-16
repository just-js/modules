#ifndef JUST_VM_H
#define JUST_VM_H

#include <just.h>

namespace just {

namespace vm {

void CompileScript(const FunctionCallbackInfo<Value> &args);
void RunModule(const FunctionCallbackInfo<Value> &args);
void RunScript(const FunctionCallbackInfo<Value> &args);
void Builtin(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_vm() {
		return (void*)just::vm::Init;
	}
}

#endif
