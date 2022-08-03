#ifndef JUST_RSYNC_H
#define JUST_RSYNC_H

#include <just.h>
#include "deps/librsync-2.3.2/src/librsync.h"

namespace just {

namespace rsync {
void Signature(const FunctionCallbackInfo<Value> &args);
void Signature2(const FunctionCallbackInfo<Value> &args);
void Delta(const FunctionCallbackInfo<Value> &args);
void Patch(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_rsync() {
		return (void*)just::rsync::Init;
	}
}

#endif
