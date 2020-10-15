#include "http.h"

void just::http::GetUrl(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int index = 0;
  int argc = args.Length();
  if (argc > 0) {
    index = args[0]->Int32Value(context).ToChecked();
  }
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, state[index].path, 
    NewStringType::kNormal, state[index].path_len).ToLocalChecked());
}

void just::http::GetMethod(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int index = 0;
  int argc = args.Length();
  if (argc > 0) {
    index = args[0]->Int32Value(context).ToChecked();
  }
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, state[index].method, 
    NewStringType::kNormal, state[index].method_len).ToLocalChecked());
}

void just::http::GetStatusCode(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int index = 0;
  int argc = args.Length();
  if (argc > 0) {
    index = args[0]->Int32Value(context).ToChecked();
  }
  args.GetReturnValue().Set(Integer::New(isolate, state[index].status));
}

void just::http::GetStatusMessage(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int index = 0;
  int argc = args.Length();
  if (argc > 0) {
    index = args[0]->Int32Value(context).ToChecked();
  }
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, state[index].status_message, 
    NewStringType::kNormal, state[index].status_message_len).ToLocalChecked());
}

void just::http::GetHeaders(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int index = 0;
  int argc = args.Length();
  if (argc > 0) {
    index = args[0]->Int32Value(context).ToChecked();
  }
  Local<Object> headers = Object::New(isolate);
  for (size_t i = 0; i < state[index].num_headers; i++) {
    struct phr_header* h = &state[index].headers[i];
    headers->Set(context, String::NewFromUtf8(isolate, h->name, 
      NewStringType::kNormal, h->name_len).ToLocalChecked(), 
      String::NewFromUtf8(isolate, h->value, NewStringType::kNormal, 
      h->value_len).ToLocalChecked()).Check();
  }
  args.GetReturnValue().Set(headers);
}

void just::http::GetRequests(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int count = 1;
  int start = 0;
  int argc = args.Length();
  if (argc > 0) {
    count = args[0]->Int32Value(context).ToChecked();
  }
  if (argc > 1) {
    start = args[1]->Int32Value(context).ToChecked();
  }
  Local<Array> requests = Array::New(isolate);
  for (int index = start; index < count; index++) {
    Local<Object> request = Object::New(isolate);
    request->Set(context, String::NewFromUtf8Literal(isolate, 
      "url"), String::NewFromUtf8(isolate, state[index].path, 
      NewStringType::kNormal, state[index].path_len).ToLocalChecked()).Check();
    request->Set(context, String::NewFromUtf8Literal(isolate, 
      "minorVersion"), Integer::New(isolate, 
      state[index].minor_version)).Check();
    request->Set(context, String::NewFromUtf8Literal(isolate, 
      "url"), String::NewFromUtf8(isolate, state[index].path, 
      NewStringType::kNormal, state[index].path_len).ToLocalChecked()).Check();
    request->Set(context, String::NewFromUtf8Literal(isolate, 
      "method"), String::NewFromUtf8(isolate, state[index].method, 
      NewStringType::kNormal, state[index].method_len).ToLocalChecked()).Check();
    Local<Object> headers = Object::New(isolate);
    for (size_t i = 0; i < state[index].num_headers; i++) {
      struct phr_header* h = &state[index].headers[i];
      headers->Set(context, String::NewFromUtf8(isolate, h->name, 
        NewStringType::kNormal, h->name_len).ToLocalChecked(), 
        String::NewFromUtf8(isolate, h->value, NewStringType::kNormal, 
        h->value_len).ToLocalChecked()).Check();
    }
    request->Set(context, String::NewFromUtf8Literal(isolate, 
      "headers"), headers).Check();
    requests->Set(context, index, request).Check();
  }
  args.GetReturnValue().Set(requests);
}

void just::http::GetResponses(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int count = 1;
  int start = 0;
  int argc = args.Length();
  if (argc > 0) {
    count = args[0]->Int32Value(context).ToChecked();
  }
  if (argc > 1) {
    start = args[1]->Int32Value(context).ToChecked();
  }
  Local<Array> responses = Array::New(isolate);
  for (int index = start; index < count; index++) {
    Local<Object> response = Object::New(isolate);
    response->Set(context, String::NewFromUtf8Literal(isolate, 
      "minorVersion"), Integer::New(isolate, 
      state[index].minor_version)).Check();
    response->Set(context, String::NewFromUtf8Literal(isolate, 
      "statusCode"), Integer::New(isolate, 
      state[index].status)).Check();
    response->Set(context, String::NewFromUtf8Literal(isolate, 
      "statusMessage"), String::NewFromUtf8(isolate, 
      state[index].status_message, NewStringType::kNormal, 
      state[index].status_message_len).ToLocalChecked()).Check();
    Local<Object> headers = Object::New(isolate);
    for (size_t i = 0; i < state[index].num_headers; i++) {
      struct phr_header* h = &state[index].headers[i];
      headers->Set(context, String::NewFromUtf8(isolate, h->name, 
        NewStringType::kNormal, h->name_len).ToLocalChecked(), 
        String::NewFromUtf8(isolate, h->value, NewStringType::kNormal, 
        h->value_len).ToLocalChecked()).Check();
    }
    response->Set(context, String::NewFromUtf8Literal(isolate, 
      "headers"), headers).Check();
    responses->Set(context, index, response).Check();
  }
  args.GetReturnValue().Set(responses);
}

void just::http::ParseRequests(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  size_t bytes = args[1]->Int32Value(context).ToChecked();
  int argc = args.Length();
  size_t off = 0;
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  just::buffer* buf = getBuffer(ab);
  char* next = (char*)buf->data + off;
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
    state[count].num_headers = JUST_MAX_HEADERS;
    nread = phr_parse_request(next, bytes, (const char **)&state[count].method, 
      &state[count].method_len, (const char **)&state[count].path, &state[count].path_len, 
      &state[count].minor_version, state[count].headers, &state[count].num_headers, 0);
  }
  if (argc > 3) {
    Local<Array> answer = args[3].As<Array>();
    answer->Set(context, 0, Integer::New(isolate, bytes)).Check();
  }
  args.GetReturnValue().Set(Integer::New(isolate, count));
}

void just::http::ParseResponses(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  size_t bytes = args[1]->Int32Value(context).ToChecked();
  int argc = args.Length();
  size_t off = 0;
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  just::buffer* buf = getBuffer(ab);
  char* next = (char*)buf->data + off;
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
    state[count].num_headers = JUST_MAX_HEADERS;
    nread = phr_parse_response(next, bytes, &state[count].minor_version, 
      &state[count].status, (const char **)&state[count].status_message, 
      &state[count].status_message_len, state[count].headers, &state[count].num_headers, 0);
  }
  if (argc > 3) {
    Local<Array> answer = args[3].As<Array>();
    answer->Set(context, 0, Integer::New(isolate, bytes)).Check();
  }
  args.GetReturnValue().Set(Integer::New(isolate, count));
}

void just::http::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> http = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, http, "parseRequests", ParseRequests);
  SET_METHOD(isolate, http, "parseResponses", ParseResponses);
  SET_METHOD(isolate, http, "getUrl", GetUrl);
  SET_METHOD(isolate, http, "getStatusCode", GetStatusCode);
  SET_METHOD(isolate, http, "getStatusMessage", GetStatusMessage);
  SET_METHOD(isolate, http, "getMethod", GetMethod);
  SET_METHOD(isolate, http, "getHeaders", GetHeaders);
  SET_METHOD(isolate, http, "getRequests", GetRequests);
  SET_METHOD(isolate, http, "getResponses", GetResponses);
  SET_VALUE(isolate, http, "MAX_PIPELINE", Integer::New(isolate, MAX_PIPELINE));
  SET_MODULE(isolate, target, "http", http);
}
