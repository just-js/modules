#ifndef JUST_BESTLINE_H
#define JUST_BESTLINE_H

#include <just.h>
#include <liburing.h>

namespace just {

namespace iouring {

struct io_data {
	int index;
	int read;
	off_t first_offset, offset;
	size_t first_len;
	struct iovec* iov;
};

void QueueInit(const FunctionCallbackInfo<Value> &args);
void QueueExit(const FunctionCallbackInfo<Value> &args);
void Submit(const FunctionCallbackInfo<Value> &args);
void GetSQE(const FunctionCallbackInfo<Value> &args);
void WaitCQE(const FunctionCallbackInfo<Value> &args);
void PeekCQE(const FunctionCallbackInfo<Value> &args);
void GetData(const FunctionCallbackInfo<Value> &args);
void CQESeen(const FunctionCallbackInfo<Value> &args);
void PrepWriteV(const FunctionCallbackInfo<Value> &args);
void PrepReadV(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_iouring() {
		return (void*)just::iouring::Init;
	}
}

#endif
