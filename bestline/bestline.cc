#include "bestline.h"

void just::bestline::BestlineHistoryLoad(const FunctionCallbackInfo<Value> &args) {
  String::Utf8Value filename(args.GetIsolate(), args[0]);
  bestlineHistoryLoad(*filename);
}

void just::bestline::BestlineHistorySave(const FunctionCallbackInfo<Value> &args) {
  String::Utf8Value filename(args.GetIsolate(), args[0]);
  bestlineHistorySave(*filename);
}

void just::bestline::BestlineHistoryAdd(const FunctionCallbackInfo<Value> &args) {
  String::Utf8Value line(args.GetIsolate(), args[0]);
  bestlineHistoryAdd(*line);
}

void just::bestline::BestlineHistoryFree(const FunctionCallbackInfo<Value> &args) {
  bestlineHistoryFree();
}

void just::bestline::BestlineClearScreen(const FunctionCallbackInfo<Value> &args) {
  bestlineClearScreen(1);
}

void just::bestline::BestlineDisableRawMode(const FunctionCallbackInfo<Value> &args) {
  bestlineDisableRawMode();
}

void just::bestline::Bestline(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value prompt(isolate, args[0]);
  char* line = bestlineChomp(*prompt);
  if (line == NULL) return;
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, line, 
    NewStringType::kNormal, strlen(line)).ToLocalChecked());
}

void just::bestline::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "bestline", Bestline);

  SET_METHOD(isolate, module, "loadHistory", BestlineHistoryLoad);
  SET_METHOD(isolate, module, "saveHistory", BestlineHistorySave);
  SET_METHOD(isolate, module, "addHistory", BestlineHistoryAdd);
  SET_METHOD(isolate, module, "clearHistory", BestlineHistoryFree);
  SET_METHOD(isolate, module, "cls", BestlineClearScreen);
  SET_METHOD(isolate, module, "disableRawMode", BestlineDisableRawMode);

  SET_MODULE(isolate, target, "bestline", module);
}
