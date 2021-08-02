#include "blake3.h"

void just::blake3::HashInit(const FunctionCallbackInfo<Value> &args) {
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> buf = ab->GetBackingStore();
  blake3_hasher* hasher = (blake3_hasher*)calloc(1, sizeof(blake3_hasher));
  blake3_hasher_init(hasher);
  ab->SetAlignedPointerInInternalField(1, hasher);
}

void just::blake3::HashInitKeyed(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> buf = ab->GetBackingStore();
  String::Utf8Value str(isolate, args[1]);
  blake3_hasher* hasher = (blake3_hasher*)calloc(1, sizeof(blake3_hasher));
  const uint8_t* key = (const uint8_t*)*str;
  blake3_hasher_init_keyed(hasher, key);
  ab->SetAlignedPointerInInternalField(1, hasher);
}

void just::blake3::HashUpdate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> outab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> out = outab->GetBackingStore();
  Local<ArrayBuffer> inab = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> in = inab->GetBackingStore();
  blake3_hasher* hasher = (blake3_hasher*)outab->GetAlignedPointerFromInternalField(1);
  unsigned int off = args[2]->Uint32Value(context).ToChecked();
  unsigned int len = args[3]->Uint32Value(context).ToChecked();
  unsigned char* source = (unsigned char*)in->Data() + off;
  blake3_hasher_update(hasher, source, len);
}

void just::blake3::HashFinalize(const FunctionCallbackInfo<Value> &args) {
  Local<ArrayBuffer> outab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> out = outab->GetBackingStore();
  blake3_hasher* hasher = (blake3_hasher*)outab->GetAlignedPointerFromInternalField(1);
  blake3_hasher_finalize(hasher, (uint8_t*)out->Data(), BLAKE3_OUT_LEN);
}

void just::blake3::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "create", HashInit);
  SET_METHOD(isolate, module, "createKeyed", HashInitKeyed);
  SET_METHOD(isolate, module, "update", HashUpdate);
  SET_METHOD(isolate, module, "finish", HashFinalize);

  SET_VALUE(isolate, module, "BLAKE3_KEY_LEN", Integer::New(isolate, BLAKE3_KEY_LEN));
  SET_VALUE(isolate, module, "BLAKE3_MAX_DEPTH", Integer::New(isolate, BLAKE3_MAX_DEPTH));
  SET_VALUE(isolate, module, "BLAKE3_OUT_LEN", Integer::New(isolate, BLAKE3_OUT_LEN));
  SET_VALUE(isolate, module, "BLAKE3_BLOCK_LEN", Integer::New(isolate, BLAKE3_BLOCK_LEN));

  SET_MODULE(isolate, target, "blake3", module);
}
