#ifndef JUST_HTTP_H
#define JUST_HTTP_H

#include "just.h"

namespace just {

namespace html {

void Escape(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_html() {
		return (void*)just::html::Init;
	}
}
#endif
