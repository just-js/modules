#ifndef JUST_IO_H
#define JUST_IO_H

#include <just.h>
#include <sys/io.h>

namespace just {

namespace io {
void IOPl(const FunctionCallbackInfo<Value> &args);
void IOPerm(const FunctionCallbackInfo<Value> &args);
void OutB(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_io() {
		return (void*)just::io::Init;
	}
}

#endif
