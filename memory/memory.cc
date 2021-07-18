#include "memory.h"

static void ResetHandleAndSetFlag(const v8::WeakCallbackInfo<just::memory::rawBuffer>& data) {
  data.GetParameter()->buffer.Reset();
  just::memory::buffers.erase(data.GetParameter()->handle);
  fprintf(stderr, "free %i\n", data.GetParameter()->handle);
  for ( const auto &p : just::memory::buffers ) {
    fprintf(stderr, "%i\n", p.first);
  } 
  delete data.GetParameter();
}

void just::memory::ReadMemory(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<BigInt> start64 = Local<BigInt>::Cast(args[0]);
  Local<BigInt> end64 = Local<BigInt>::Cast(args[1]);
  const uint64_t size = end64->Uint64Value() - start64->Uint64Value();
  void* start = reinterpret_cast<void*>(start64->Uint64Value());
  std::unique_ptr<BackingStore> backing = ArrayBuffer::NewBackingStore(
      start, size, [](void*, size_t, void*){}, nullptr);
  Local<SharedArrayBuffer> ab = SharedArrayBuffer::New(isolate, std::move(backing));
  //Local<ArrayBuffer> ab = ArrayBuffer::New(isolate, start, size, v8::ArrayBufferCreationMode::kExternalized);
  args.GetReturnValue().Set(ab);
}

void just::memory::GetMeta(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> meta = args[1].As<Object>();
  bool isExternal;
  bool isDetachable;
  if (args[0]->IsArrayBuffer()) {
    Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
    isExternal = buf->IsExternal();
    isDetachable = buf->IsExternal();
  } else if (args[0]->IsSharedArrayBuffer()) {
    Local<SharedArrayBuffer> buf = args[0].As<SharedArrayBuffer>();
    isExternal = buf->IsExternal();
    isDetachable = buf->IsExternal();
  }
  meta->Set(context, String::NewFromUtf8Literal(isolate, "isExternal", NewStringType::kNormal), v8::Boolean::New(isolate, isExternal)).Check();
  meta->Set(context, String::NewFromUtf8Literal(isolate, "isDetachable", NewStringType::kNormal), v8::Boolean::New(isolate, isDetachable)).Check();
  args.GetReturnValue().Set(meta);
}

void just::memory::RawBuffer(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  just::memory::rawBuffer* buf = new just::memory::rawBuffer();
  buf->data = backing->Data();
  buf->len = backing->ByteLength();
  buf->buffer.Reset(isolate, ab);
  buf->handle = just::memory::bcount;
  buf->buffer.SetWeak(buf, &ResetHandleAndSetFlag, v8::WeakCallbackType::kFinalizer);
  just::memory::buffers[just::memory::bcount] = buf;
  args.GetReturnValue().Set(Integer::New(isolate, just::memory::bcount++));
}

void just::memory::Alloc(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int size = Local<Integer>::Cast(args[0])->Value();
  Local<ArrayBuffer> ab = ArrayBuffer::New(isolate, size);
  just::memory::rawBuffer* buf = new just::memory::rawBuffer();
  buf->data = ab->GetBackingStore()->Data();
  buf->len = size;
  buf->buffer.Reset(isolate, ab);
  buf->handle = just::memory::bcount;
  buf->buffer.SetWeak(buf, &ResetHandleAndSetFlag, v8::WeakCallbackType::kFinalizer);
  just::memory::buffers[just::memory::bcount] = buf;
  ab->Set(context, String::NewFromUtf8Literal(isolate, "raw", NewStringType::kNormal), Integer::New(isolate, just::memory::bcount++)).Check();
  args.GetReturnValue().Set(ab);
}

void just::memory::WriteString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  just::memory::rawBuffer* b = just::memory::buffers[Local<Integer>::Cast(args[0])->Value()];
  Local<String> str = args[1].As<String>();
  int len = str->Utf8Length(isolate);
  int nchars = 0;
  int off = 0;
  if (args.Length() > 2) {
    off = Local<Integer>::Cast(args[2])->Value();
  }
  char* dest = (char*)b->data + off;
  args.GetReturnValue().Set(Integer::New(isolate, str->WriteUtf8(isolate, dest, len, &nchars, v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION)));
}

void just::memory::WriteCString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  just::memory::rawBuffer* b = just::memory::buffers[Local<Integer>::Cast(args[0])->Value()];
  Local<String> str = args[1].As<String>();
  int len = str->Length();
  int off = 0;
  if (args.Length() > 2) off = Local<Integer>::Cast(args[2])->Value();
  args.GetReturnValue().Set(Integer::New(isolate, str->WriteOneByte(isolate, (uint8_t*)b->data, off, len, v8::String::HINT_MANY_WRITES_EXPECTED)));
}

void just::memory::WritePointer(const FunctionCallbackInfo<Value> &args) {
  just::memory::rawBuffer* dest = just::memory::buffers[Local<Integer>::Cast(args[0])->Value()];
  int off = Local<Integer>::Cast(args[1])->Value();
  just::memory::rawBuffer* src = just::memory::buffers[Local<Integer>::Cast(args[2])->Value()];
  char* ptr = (char*)dest->data + off;
  *reinterpret_cast<void **>(ptr) = src->data;
}

void just::memory::ReadString(const FunctionCallbackInfo<Value> &args) {
  just::memory::rawBuffer* b = just::memory::buffers[Local<Integer>::Cast(args[0])->Value()];
  int len = b->len;
  int argc = args.Length();
  if (argc > 1) {
    len = Local<Integer>::Cast(args[1])->Value();
  }
  int off = 0;
  if (argc > 2) {
    off = Local<Integer>::Cast(args[2])->Value();
  }
  const char* src = (const char*)b->data + off;
  args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), src, 
    NewStringType::kNormal, len).ToLocalChecked());
}

void just::memory::Copy(const FunctionCallbackInfo<Value> &args) {
  just::memory::rawBuffer* dest = just::memory::buffers[Local<Integer>::Cast(args[0])->Value()];
  just::memory::rawBuffer* src = just::memory::buffers[Local<Integer>::Cast(args[1])->Value()];
  int argc = args.Length();
  int doff = 0;
  if (argc > 2) {
    doff = Local<Integer>::Cast(args[2])->Value();
  }
  int len = src->len;
  if (argc > 3) {
    len = Local<Integer>::Cast(args[3])->Value();
  }
  int soff = 0;
  if (argc > 4) {
    soff = Local<Integer>::Cast(args[4])->Value();
  }
  char* d = (char*)dest->data + doff;
  const char* s = (const char*)src->data + soff;
  memcpy(d, s, len);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), len));
}

void just::memory::GetAddress(const FunctionCallbackInfo<Value> &args) {
  just::memory::rawBuffer* b = just::memory::buffers[Local<Integer>::Cast(args[0])->Value()];
  args.GetReturnValue().Set(BigInt::New(args.GetIsolate(), (uint64_t)b->data));
}

void just::memory::MemFdCreate(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value fname(isolate, args[0]);
  int flags = Local<Integer>::Cast(args[1])->Value();
  args.GetReturnValue().Set(Integer::New(isolate, memfd_create(*fname, flags)));
}

void just::memory::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "readString", ReadString);
  SET_METHOD(isolate, module, "writePointer", WritePointer);
  SET_METHOD(isolate, module, "writeString", WriteString);
  SET_METHOD(isolate, module, "writeCString", WriteCString);
  SET_METHOD(isolate, module, "getAddress", GetAddress);
  SET_METHOD(isolate, module, "rawBuffer", RawBuffer);
  SET_METHOD(isolate, module, "readMemory", ReadMemory);
  SET_METHOD(isolate, module, "getMeta", GetMeta);
  SET_METHOD(isolate, module, "memfdCreate", MemFdCreate);
  SET_METHOD(isolate, module, "copy", Copy);
  SET_METHOD(isolate, module, "alloc", Alloc);
  SET_VALUE(isolate, module, "MFD_CLOEXEC", Integer::New(isolate, MFD_CLOEXEC));
  SET_MODULE(isolate, target, "memory", module);
}
