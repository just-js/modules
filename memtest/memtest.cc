#include "memtest.h"

void* just::memtest::getBufferData (Local<Value> arg) {
  Local<ArrayBuffer> buf = arg.As<ArrayBuffer>();
  void* data = buf->GetAlignedPointerFromInternalField(1);
  if (!data) {
    std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
    data = backing->Data();
    buf->SetAlignedPointerInInternalField(1, data);
  }
  return data;
}

void just::memtest::ReadOne(const FunctionCallbackInfo<Value> &args) {
  int len = Local<Integer>::Cast(args[1])->Value();
  const char* dest = (const char*)getBufferData(args[0]);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), len));
}

void just::memtest::ReadTwo(const FunctionCallbackInfo<Value> &args) {
  Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  int len = backing->ByteLength();
  const char* data = (const char*)backing->Data();
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), len));
}

void just::memtest::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "readOne", ReadOne);
  SET_METHOD(isolate, module, "readTwo", ReadTwo);
  SET_MODULE(isolate, target, "memtest", module);
}
