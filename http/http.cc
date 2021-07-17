#include "http.h"

void just::http::GetUrl(const FunctionCallbackInfo<Value> &args) {
  int index = Local<Integer>::Cast(args[0])->Value();
  args.GetReturnValue().Set(String::NewFromOneByte(args.GetIsolate(), (const uint8_t*)state[index].path, 
    NewStringType::kNormal, state[index].path_len).ToLocalChecked());
}

void just::http::GetMethod(const FunctionCallbackInfo<Value> &args) {
  int index = Local<Integer>::Cast(args[0])->Value();
  args.GetReturnValue().Set(String::NewFromOneByte(args.GetIsolate(), (const uint8_t*)state[index].method, 
    NewStringType::kInternalized, state[index].method_len).ToLocalChecked());
}

void just::http::GetMethodAndUrl(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  int index = Local<Integer>::Cast(args[0])->Value();
  Local<Array> headers = args[1].As<Array>();
  Local<Context> context = isolate->GetCurrentContext();
  headers->Set(context, 0, 
    String::NewFromOneByte(isolate, (const uint8_t*)state[index].method, NewStringType::kInternalized, 
    state[index].method_len).ToLocalChecked()).Check();
  headers->Set(context, 1, 
    String::NewFromOneByte(isolate, (const uint8_t*)state[index].path, NewStringType::kNormal, 
    state[index].path_len).ToLocalChecked()).Check();
}

void just::http::GetStatusCode(const FunctionCallbackInfo<Value> &args) {
  int index = Local<Integer>::Cast(args[0])->Value();
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), state[index].status));
}

void just::http::GetVersion(const FunctionCallbackInfo<Value> &args) {
  int index = Local<Integer>::Cast(args[0])->Value();
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), state[index].minor_version));
}

void just::http::GetStatusMessage(const FunctionCallbackInfo<Value> &args) {
  int index = Local<Integer>::Cast(args[0])->Value();
  args.GetReturnValue().Set(String::NewFromOneByte(args.GetIsolate(), (const uint8_t*)state[index].status_message, 
    NewStringType::kInternalized, state[index].status_message_len).ToLocalChecked());
}

void just::http::GetHeaders(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int index = Local<Integer>::Cast(args[0])->Value();
  Local<Object> headers = args[1].As<Object>();
  for (size_t i = 0; i < state[index].num_headers; i++) {
    struct phr_header* h = &state[index].headers[i];
    headers->Set(context, String::NewFromOneByte(isolate, (const uint8_t*)h->name, 
      NewStringType::kInternalized, h->name_len).ToLocalChecked(), 
      String::NewFromOneByte(isolate, (const uint8_t*)h->value, NewStringType::kNormal, 
      h->value_len).ToLocalChecked()).Check();
  }
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), state[index].minor_version));
}

void just::http::GetRequests(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int count = Local<Integer>::Cast(args[0])->Value();
  Local<Array> requests = args[1].As<Array>();
  for (int index = 0; index < count; index++) {
    Local<Array> request = 
      requests->Get(context, index).ToLocalChecked().As<Array>();
    request->Set(context, 0, String::NewFromOneByte(isolate, (const uint8_t*)state[index].path, 
      NewStringType::kNormal, state[index].path_len).ToLocalChecked()).Check();
    request->Set(context, 1, Integer::New(isolate, 
      state[index].minor_version)).Check();
    request->Set(context, 2, String::NewFromOneByte(isolate, (const uint8_t*)state[index].method, 
      NewStringType::kInternalized, state[index].method_len).ToLocalChecked()).Check();
    Local<Object> headers = Object::New(isolate);
    request->Set(context, 3, headers).Check();
    for (size_t i = 0; i < state[index].num_headers; i++) {
      struct phr_header* h = &state[index].headers[i];
      headers->Set(context, String::NewFromOneByte(isolate, (const uint8_t*)h->name, 
        NewStringType::kInternalized, h->name_len).ToLocalChecked(), 
        String::NewFromOneByte(isolate, (const uint8_t*)h->value, NewStringType::kNormal, 
        h->value_len).ToLocalChecked()).Check();
    }
  }
}

void just::http::GetResponses(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int count = Local<Integer>::Cast(args[0])->Value();
  Local<Array> responses = args[1].As<Array>();
  for (int index = 0; index < count; index++) {
    Local<Array> response = 
      responses->Get(context, index).ToLocalChecked().As<Array>();
    response->Set(context, 0, Integer::New(isolate, 
      state[index].minor_version)).Check();
    response->Set(context, 1, Integer::New(isolate, 
      state[index].status)).Check();
    response->Set(context, 2, String::NewFromOneByte(isolate, 
      (const uint8_t*)state[index].status_message, NewStringType::kInternalized, 
      state[index].status_message_len).ToLocalChecked()).Check();
    Local<Object> headers = Object::New(isolate);
    response->Set(context, 3, headers).Check();
    for (size_t i = 0; i < state[index].num_headers; i++) {
      struct phr_header* h = &state[index].headers[i];
      headers->Set(context, String::NewFromOneByte(isolate, (const uint8_t*)h->name, 
        NewStringType::kInternalized, h->name_len).ToLocalChecked(), 
        String::NewFromOneByte(isolate, (const uint8_t*)h->value, NewStringType::kNormal, 
        h->value_len).ToLocalChecked()).Check();
    }
  }
}

void just::http::ParseRequests(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
  size_t bytes = Local<Integer>::Cast(args[1])->Value();
  size_t off = Local<Integer>::Cast(args[2])->Value();
  void* data = buf->GetAlignedPointerFromInternalField(1);
  if (!data) {
    std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
    void* data = backing->Data();
    buf->SetAlignedPointerInInternalField(1, data);
  }
  char* next = (char*)data + off;
  int count = 0;
  state[count].num_headers = JUST_MAX_HEADERS;
  //TODO: buffer overrun
  ssize_t nread = phr_parse_request(next, bytes, (const char **)&state[count].method, 
    &state[count].method_len, (const char **)&state[count].path, &state[count].path_len, 
    &state[count].minor_version, state[count].headers, &state[count].num_headers, 0);
  while (nread > -1) {
    count++;
    next += nread;
    bytes -= nread;
    // TODO: check next < buffer size
    if (bytes <= 0) break;
    state[count].num_headers = JUST_MAX_HEADERS;
    nread = phr_parse_request(next, bytes, (const char **)&state[count].method, 
      &state[count].method_len, (const char **)&state[count].path, &state[count].path_len, 
      &state[count].minor_version, state[count].headers, &state[count].num_headers, 0);
  }
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> answer = args[3].As<Array>();
  answer->Set(context, 0, Integer::New(isolate, bytes)).Check();
  answer->Set(context, 1, Integer::New(isolate, count)).Check();
  args.GetReturnValue().Set(answer);
}

void just::http::ParseResponses(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
  size_t bytes = Local<Integer>::Cast(args[1])->Value();
  size_t off = Local<Integer>::Cast(args[2])->Value();
  void* data = buf->GetAlignedPointerFromInternalField(1);
  if (!data) {
    std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
    void* data = backing->Data();
    buf->SetAlignedPointerInInternalField(1, data);
  }
  char* next = (char*)data + off;
  int count = 0;
  state[count].num_headers = JUST_MAX_HEADERS;
  //TODO: buffer overrun
  ssize_t nread = phr_parse_response(next, bytes, &state[count].minor_version, 
    &state[count].status, (const char **)&state[count].status_message, 
    &state[count].status_message_len, state[count].headers, &state[count].num_headers, 0);
  while (nread > -1) {
    count++;
    next += nread;
    bytes -= nread;
    if (bytes <= 0) break;
    state[count].num_headers = JUST_MAX_HEADERS;
    nread = phr_parse_response(next, bytes, &state[count].minor_version, 
      &state[count].status, (const char **)&state[count].status_message, 
      &state[count].status_message_len, state[count].headers, &state[count].num_headers, 0);
  }
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> answer = args[3].As<Array>();
  answer->Set(context, 0, Integer::New(isolate, bytes)).Check();
  answer->Set(context, 1, Integer::New(isolate, count)).Check();
  args.GetReturnValue().Set(answer);
}

void just::http::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> http = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, http, "parseRequests", ParseRequests);
  SET_METHOD(isolate, http, "parseResponses", ParseResponses);
  SET_METHOD(isolate, http, "getUrl", GetUrl);
  SET_METHOD(isolate, http, "getVersion", GetVersion);
  SET_METHOD(isolate, http, "getStatusCode", GetStatusCode);
  SET_METHOD(isolate, http, "getStatusMessage", GetStatusMessage);
  SET_METHOD(isolate, http, "getMethod", GetMethod);
  SET_METHOD(isolate, http, "getMethodAndUrl", GetMethodAndUrl);
  SET_METHOD(isolate, http, "getHeaders", GetHeaders);
  SET_METHOD(isolate, http, "getRequests", GetRequests);
  SET_METHOD(isolate, http, "getResponses", GetResponses);
  SET_VALUE(isolate, http, "MAX_PIPELINE", Integer::New(isolate, MAX_PIPELINE));
  SET_VALUE(isolate, http, "MAX_HEADERS", Integer::New(isolate, JUST_MAX_HEADERS));
  SET_MODULE(isolate, target, "http", http);
}
