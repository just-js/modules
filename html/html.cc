#include "html.h"

/*
void just::sys::FindFast(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  Local<ArrayBuffer> rbuf = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> results = rbuf->GetBackingStore();
  int argc = args.Length();
  size_t bytes = backing->ByteLength();
  if (argc > 2) {
    bytes = args[2]->Int32Value(context).ToChecked();
  }
  size_t off = 0;
  if (argc > 3) {
    off = args[3]->Int32Value(context).ToChecked();
  }
  const char* next = (const char*)backing->Data() + off;
  const char* needle = "\r\n\r\n";
  uint32_t* offsets = (uint32_t*)results->Data();
  int nlen = 4;
  size_t end = bytes + off;
  __m128i needle16 = _mm_loadu_si128((const __m128i *)needle);
  int count = 0;
  int orig = off;
  int r = 0;
  __m128i haystack16;
  while (off < end) {
    haystack16 = _mm_loadu_si128((const __m128i *)next);
    r = _mm_cmpestri(needle16, nlen, haystack16, 16, _SIDD_CMP_EQUAL_ORDERED | _SIDD_UBYTE_OPS);
    if (r < (16 - nlen)) {
      offsets[count++] = r + off + nlen;
    }
    off += 16 - nlen;
    next += 16 - nlen;
  }
  offsets[count] = orig + bytes;
  args.GetReturnValue().Set(Integer::New(isolate, count));
}
*/

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
  if (size > len) {
    EscapedString* source = new EscapedString(dest, size);
    args.GetReturnValue().Set(v8::String::NewExternalOneByte(isolate, source).ToLocalChecked());
    return;
  }
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, (const char*)dest, 
    NewStringType::kNormal, size).ToLocalChecked());
}

void just::html::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> html = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, html, "escape", Escape);
  SET_MODULE(isolate, target, "html", html);
}
