#ifndef JUST_PG_H
#define JUST_PG_H

#include <just.h>
#include <libpq-fe.h>
#include <backend/catalog/pg_type_d.h>

namespace just {

namespace pg {

void Connect(const FunctionCallbackInfo<Value> &args);
void Exec(const FunctionCallbackInfo<Value> &args);
void Prepare(const FunctionCallbackInfo<Value> &args);
void ExecPrepared(const FunctionCallbackInfo<Value> &args);
void Close(const FunctionCallbackInfo<Value> &args);
void Clear(const FunctionCallbackInfo<Value> &args);
void GetValue(const FunctionCallbackInfo<Value> &args);
void GetType(const FunctionCallbackInfo<Value> &args);
void GetSize(const FunctionCallbackInfo<Value> &args);
void GetFieldName(const FunctionCallbackInfo<Value> &args);
void CommandStatus(const FunctionCallbackInfo<Value> &args);
void ResultStatus(const FunctionCallbackInfo<Value> &args);
void ErrorMessage(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_pg() {
		return (void*)just::pg::Init;
	}
}
#endif
