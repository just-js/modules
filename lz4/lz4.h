#ifndef JUST_ZLIB_H
#define JUST_ZLIB_H

#include "just.h"
#include <zconf.h>
#include <lz4.h>

namespace just {

namespace lz4 {

void CompressBound(const FunctionCallbackInfo<Value> &args);
void Compress(const FunctionCallbackInfo<Value> &args);
void Decompress(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_lz4() {
		return (void*)just::lz4::Init;
	}
}

#endif
