#ifndef JUST_THREAD_H
#define JUST_THREAD_H

#include <just.h>

namespace just {

namespace thread {

struct threadContext {
  int argc;
  char** argv;
  char* source;
  struct iovec buf;
  int fd;
  unsigned int source_len;
  char* main;
  unsigned int main_len;
};

void* startThread(void *data);

void Spawn(const FunctionCallbackInfo<Value> &args);
void Join(const FunctionCallbackInfo<Value> &args);
void TryJoin(const FunctionCallbackInfo<Value> &args);
void Self(const FunctionCallbackInfo<Value> &args);
void SetAffinity(const FunctionCallbackInfo<Value> &args);
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
