#ifndef JUST_SQLITE_H
#define JUST_SQLITE_H

#include "just.h"
#include <sqlite3.h>

namespace just {
// https://zetcode.com/db/sqlitec/
// https://github.com/sql-js/sql.js/blob/master/src/exported_functions.json
namespace sqlite {

void Version(const FunctionCallbackInfo<Value> &args);
void Open(const FunctionCallbackInfo<Value> &args);
void Prepare(const FunctionCallbackInfo<Value> &args);
void Error(const FunctionCallbackInfo<Value> &args);
void Step(const FunctionCallbackInfo<Value> &args);
void ColumnText(const FunctionCallbackInfo<Value> &args);
void ColumnInt(const FunctionCallbackInfo<Value> &args);
void ColumnInt64(const FunctionCallbackInfo<Value> &args);
void ColumnDouble(const FunctionCallbackInfo<Value> &args);
void ColumnBlob(const FunctionCallbackInfo<Value> &args);
void ColumnName(const FunctionCallbackInfo<Value> &args);
void ColumnType(const FunctionCallbackInfo<Value> &args);
void ColumnBytes(const FunctionCallbackInfo<Value> &args);
void ColumnCount(const FunctionCallbackInfo<Value> &args);
void Finalize(const FunctionCallbackInfo<Value> &args);
void Reset(const FunctionCallbackInfo<Value> &args);
void Close(const FunctionCallbackInfo<Value> &args);
void Initialize(const FunctionCallbackInfo<Value> &args);
void Shutdown(const FunctionCallbackInfo<Value> &args);
void Exec(const FunctionCallbackInfo<Value> &args);
void LastId(const FunctionCallbackInfo<Value> &args);
void FindVFS(const FunctionCallbackInfo<Value> &args);
void RegisterVFS(const FunctionCallbackInfo<Value> &args);
void UnregisterVFS(const FunctionCallbackInfo<Value> &args);
void Config(const FunctionCallbackInfo<Value> &args);
void EnableShared(const FunctionCallbackInfo<Value> &args);
void MemoryUsed(const FunctionCallbackInfo<Value> &args);
void MemoryHighwater(const FunctionCallbackInfo<Value> &args);
void ReleaseDBMemory(const FunctionCallbackInfo<Value> &args);
void ReleaseMemory(const FunctionCallbackInfo<Value> &args);
void ErrCode(const FunctionCallbackInfo<Value> &args);
void ErrMessage(const FunctionCallbackInfo<Value> &args);

void BindInt(const FunctionCallbackInfo<Value> &args);
void BindInt64(const FunctionCallbackInfo<Value> &args);
void BindDouble(const FunctionCallbackInfo<Value> &args);
void BindBlob(const FunctionCallbackInfo<Value> &args);
void BindText(const FunctionCallbackInfo<Value> &args);
void ParamIndex(const FunctionCallbackInfo<Value> &args);

void AutoCommit(const FunctionCallbackInfo<Value> &args);
void ThreadSafe(const FunctionCallbackInfo<Value> &args);
void CheckPoint(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_sqlite() {
		return (void*)just::sqlite::Init;
	}
}

#endif
