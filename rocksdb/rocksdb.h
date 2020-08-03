#ifndef JUST_ROCKSDB_H
#define JUST_ROCKSDB_H

#include "just.h"
#include <rocksdb/c.h>

namespace just {

namespace rocksdb {

void FreeMemory(void* buf, size_t length, void* data);
void Close(const FunctionCallbackInfo<Value> &args);
void Get(const FunctionCallbackInfo<Value> &args);
void Put(const FunctionCallbackInfo<Value> &args);
void OpenBackupEngine(const FunctionCallbackInfo<Value> &args);
void CreateReadOptions(const FunctionCallbackInfo<Value> &args);
void CreateWriteOptions(const FunctionCallbackInfo<Value> &args);
void DestroyRestoreOptions(const FunctionCallbackInfo<Value> &args);
void DestroyReadOptions(const FunctionCallbackInfo<Value> &args);
void DestroyWriteOptions(const FunctionCallbackInfo<Value> &args);
void DestroyOptions(const FunctionCallbackInfo<Value> &args);
void CloseBackupEngine(const FunctionCallbackInfo<Value> &args);
void RestoreFromBackup(const FunctionCallbackInfo<Value> &args);
void CreateRestoreOptions(const FunctionCallbackInfo<Value> &args);
void CreateNewBackup(const FunctionCallbackInfo<Value> &args);
void CreateOptions(const FunctionCallbackInfo<Value> &args);
void IncreaseParallelism(const FunctionCallbackInfo<Value> &args);
void CreateIfMissing(const FunctionCallbackInfo<Value> &args);
void OptimizeLevelStyleCompaction(const FunctionCallbackInfo<Value> &args);
void OptimizeUniversalStyleCompaction(const FunctionCallbackInfo<Value> &args);
void Open(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

#ifdef SHARED
extern "C" {
	void* _register_rocksdb() {
		return (void*)just::rocksdb::Init;
	}
}
#endif
#endif
