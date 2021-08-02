#ifndef JUST_THREAD_H
#define JUST_THREAD_H

#include <just.h>

namespace just {

namespace thread {

struct threadContext {
  int argc;
  int fd;
  unsigned int source_len;
  unsigned int main_len;
  char* source;
  char* main;
  char** argv;
  struct iovec buf;
};

void* startThread(void *data);

void Spawn(const FunctionCallbackInfo<Value> &args);
void Join(const FunctionCallbackInfo<Value> &args);
void Cancel(const FunctionCallbackInfo<Value> &args);
void TryJoin(const FunctionCallbackInfo<Value> &args);
void Self(const FunctionCallbackInfo<Value> &args);
void SetAffinity(const FunctionCallbackInfo<Value> &args);
void SetName(const FunctionCallbackInfo<Value> &args);
void GetAffinity(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_thread() {
		return (void*)just::thread::Init;
	}
}

#endif
