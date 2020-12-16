#ifndef JUST_FS_H
#define JUST_FS_H

#include <just.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>

namespace just {

namespace fs {

void ReadFile(const FunctionCallbackInfo<Value> &args);
void Unlink(const FunctionCallbackInfo<Value> &args);
void Symlink(const FunctionCallbackInfo<Value> &args);
void Open(const FunctionCallbackInfo<Value> &args);
void Ioctl(const FunctionCallbackInfo<Value> &args);
void Fstat(const FunctionCallbackInfo<Value> &args);
void Ftruncate(const FunctionCallbackInfo<Value> &args);
void Rmdir(const FunctionCallbackInfo<Value> &args);
void Chdir(const FunctionCallbackInfo<Value> &args);
void Rename(const FunctionCallbackInfo<Value> &args);
void Mkdir(const FunctionCallbackInfo<Value> &args);
void Readdir(const FunctionCallbackInfo<Value> &args);
void Lseek(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_fs() {
		return (void*)just::fs::Init;
	}
}

#endif
