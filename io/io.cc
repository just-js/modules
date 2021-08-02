#include "io.h"

void just::io::IOPl(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int level = Local<Integer>::Cast(args[0])->Value();
  args.GetReturnValue().Set(Integer::New(isolate, iopl(level)));
}

void just::io::IOPerm(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int from = Local<Integer>::Cast(args[0])->Value();
  uint64_t num = Local<BigInt>::Cast(args[1])->Uint64Value();
  int turn_on = Local<Integer>::Cast(args[2])->Value();
  args.GetReturnValue().Set(Integer::New(isolate, ioperm(from, num, turn_on)));
}

void just::io::OutB(const FunctionCallbackInfo<Value> &args) {
  unsigned char value = Local<Integer>::Cast(args[0])->Value();
  unsigned short int port = Local<Integer>::Cast(args[1])->Value();
  outb(value, port);
}

void just::io::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "iopl", IOPl);
  SET_METHOD(isolate, module, "ioperm", IOPerm);
  SET_METHOD(isolate, module, "outb", OutB);
  SET_MODULE(isolate, target, "io", module);
}
