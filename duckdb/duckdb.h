#ifndef JUST_SQLITE_H
#define JUST_SQLITE_H

#include "just.h"
#include "deps/duckdb.h"

namespace just {

namespace duckdb {

void CreateConfig(const FunctionCallbackInfo<Value> &args);
void SetConfig(const FunctionCallbackInfo<Value> &args);
void Open(const FunctionCallbackInfo<Value> &args);
void Connect(const FunctionCallbackInfo<Value> &args);
void Query(const FunctionCallbackInfo<Value> &args);

void RowCount(const FunctionCallbackInfo<Value> &args);
void ColumnCount(const FunctionCallbackInfo<Value> &args);
void ValueVarchar(const FunctionCallbackInfo<Value> &args);
void Free(const FunctionCallbackInfo<Value> &args);
void DestroyResult(const FunctionCallbackInfo<Value> &args);
void DestroyConfig(const FunctionCallbackInfo<Value> &args);
void Disconnect(const FunctionCallbackInfo<Value> &args);
void Close(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_duckdb() {
		return (void*)just::duckdb::Init;
	}
}

#endif
