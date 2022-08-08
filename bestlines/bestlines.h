#ifndef JUST_BESTLINE_H
#define JUST_BESTLINE_H

#include <just.h>

extern "C" {
#include "deps/bestline-master/bestline.h"
}

namespace just {

namespace bestlines {
void Bestline(const FunctionCallbackInfo<Value> &args);

void BestlineHistoryLoad(const FunctionCallbackInfo<Value> &args);
void BestlineHistorySave(const FunctionCallbackInfo<Value> &args);
void BestlineHistoryAdd(const FunctionCallbackInfo<Value> &args);
void BestlineHistoryFree(const FunctionCallbackInfo<Value> &args);
void BestlineClearScreen(const FunctionCallbackInfo<Value> &args);

void BestlineDisableRawMode(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_bestlines() {
		return (void*)just::bestlines::Init;
	}
}

#endif
