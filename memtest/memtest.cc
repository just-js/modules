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

void just::memtest::ReadArrayBufferFast(const FunctionCallbackInfo<Value> &args) {
  Local<Object> ab = args[0].As<Object>();
  void* data = ab->GetAlignedPointerFromInternalField(8);
  if (!data) data = args[0].As<ArrayBuffer>()->GetBackingStore()->Data();
  char *chunk = static_cast<char *>(data);
  //char *chunk = static_cast<char *>(args[0].As<ArrayBuffer>()->GetAlignedPointerFromInternalField(0));
}

void just::memtest::ReadArrayBuffer(const FunctionCallbackInfo<Value> &args) {
  char *chunk = static_cast<char *>(args[0].As<ArrayBuffer>()->GetBackingStore()->Data());
}

void just::memtest::ReadBufferObject(const FunctionCallbackInfo<Value> &args) {
  char* chunk = static_cast<char *>(args[0].As<Object>()->GetAlignedPointerFromInternalField(8));
}

void just::memtest::CreateBufferObject(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int size = Local<Integer>::Cast(args[0])->Value();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> optionsTemplate = ObjectTemplate::New(isolate);
  optionsTemplate->SetInternalFieldCount(10);
  Local<Object> obj = optionsTemplate->NewInstance(context).ToLocalChecked();
  obj->SetAlignedPointerInInternalField(8, calloc(1, size));
  args.GetReturnValue().Set(obj);
}

void just::memtest::CreateArrayBuffer(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int size = Local<Integer>::Cast(args[0])->Value();
  Local<ArrayBuffer> ab = ArrayBuffer::New(isolate, size);
  args.GetReturnValue().Set(ab);
}

void just::memtest::CreateArrayBufferInternal(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int size = Local<Integer>::Cast(args[0])->Value();
  void* chunk = calloc(1, size);
  std::unique_ptr<BackingStore> out = ArrayBuffer::NewBackingStore(chunk, size, just::FreeMemory, nullptr);
  Local<ArrayBuffer> ab = ArrayBuffer::New(isolate, std::move(out));
  ab->SetAlignedPointerInInternalField(0, chunk);
  args.GetReturnValue().Set(ab);
}

void just::memtest::CreateExternalBuffer(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int size = Local<Integer>::Cast(args[0])->Value();
  std::unique_ptr<BackingStore> out = ArrayBuffer::NewBackingStore(calloc(1, size), size, just::FreeMemory, nullptr);
  args.GetReturnValue().Set(ArrayBuffer::New(isolate, std::move(out)));
}

void just::memtest::CreateExternalStaticBuffer(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int size = Local<Integer>::Cast(args[0])->Value();
  std::unique_ptr<BackingStore> out = ArrayBuffer::NewBackingStore(calloc(1, size), size, [](void*, size_t, void*){}, nullptr);
  args.GetReturnValue().Set(ArrayBuffer::New(isolate, std::move(out)));
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

  SET_METHOD(isolate, module, "readArrayBuffer", ReadArrayBuffer);
  SET_METHOD(isolate, module, "readBufferObject", ReadBufferObject);
  SET_METHOD(isolate, module, "readArrayBufferFast", ReadArrayBufferFast);

  SET_METHOD(isolate, module, "createBufferObject", CreateBufferObject);
  SET_METHOD(isolate, module, "createArrayBuffer", CreateArrayBuffer);
  SET_METHOD(isolate, module, "createArrayBufferInternal", CreateArrayBufferInternal);
  SET_METHOD(isolate, module, "createExternalBuffer", CreateExternalBuffer);
  SET_METHOD(isolate, module, "createExternalStaticBuffer", CreateExternalStaticBuffer);

  SET_MODULE(isolate, target, "memtest", module);
}
