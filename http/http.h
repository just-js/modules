#ifndef JUST_HTTP_H
#define JUST_HTTP_H

#include "just.h"
#include <picohttpparser.h>

namespace just {

namespace http {

typedef struct httpContext httpContext;
#define MAX_PIPELINE 4096
#define JUST_MAX_HEADERS 32

struct httpContext {
  int minor_version;
  int status;
  size_t status_message_len;
  size_t method_len;
  size_t path_len;
  uint32_t body_length;
  uint32_t body_bytes;
  uint16_t header_size;
  size_t num_headers;
  struct phr_header headers[JUST_MAX_HEADERS];
  const char* path;
  const char* method;
  const char* status_message;
};

thread_local httpContext state[MAX_PIPELINE];

void GetUrl(const FunctionCallbackInfo<Value> &args);
void GetVersion(const FunctionCallbackInfo<Value> &args);
void GetMethod(const FunctionCallbackInfo<Value> &args);
void GetMethodAndUrl(const FunctionCallbackInfo<Value> &args);
void GetStatusCode(const FunctionCallbackInfo<Value> &args);
void GetStatusMessage(const FunctionCallbackInfo<Value> &args);
void GetHeaders(const FunctionCallbackInfo<Value> &args);
void GetRequests(const FunctionCallbackInfo<Value> &args);
void GetResponses(const FunctionCallbackInfo<Value> &args);
void ParseRequests(const FunctionCallbackInfo<Value> &args);
void ParseResponses(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_http() {
		return (void*)just::http::Init;
	}
}
#endif
