#include "html.h"

void just::html::Escape(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  const uint8_t* data = static_cast<uint8_t *>(backing->Data());
  size_t len = backing->ByteLength();
  int argc = args.Length();
  if (argc > 1) {
    len = args[1]->Int32Value(context).ToChecked();
  }
  int off = 0;
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  const uint8_t* source = data + off;
  uint8_t* dest;
  size_t size = hesc_escape_html(&dest, source, len);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, (const char*)dest, 
    NewStringType::kNormal, size).ToLocalChecked());
}

void just::html::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> html = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, html, "escape", Escape);
  SET_MODULE(isolate, target, "html", html);
}
