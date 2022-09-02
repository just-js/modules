#include "html.h"

std::string htmlTranslate(const char *str, size_t length)
{
  std::string ret;
  ret.reserve(length + 64);
  auto end = str + length;
  while (str != end) {
    switch (*str) {
      case '"':
        ret.append("&quot;", 6);
        break;
      case '&':
        ret.append("&amp;", 5);
        break;
      case '\'':
        ret.append("&#x27;", 6);
        break;
      case '<':
        ret.append("&lt;", 4);
        break;
      case '>':
        ret.append("&gt;", 4);
        break;
      default:
        ret.push_back(*str);
        break;
    }
    ++str;
  }
  return ret;
}

static bool needTranslation(const char *str, size_t length) {
  for (size_t i = 0; i < length; i++) {
    switch (str[i]) {
      case '"':
      case '&':
      case '<':
      case '>':
          return true;
      default:
          continue;
    }
  }
  return false;
}

void just::html::Escape(const FunctionCallbackInfo<Value> &args) {
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  const char* data = static_cast<char *>(backing->Data());
  size_t len = 0;
  int argc = args.Length();
  if (argc > 1) {
    len = Local<Integer>::Cast(args[1])->Value();
  } else {
    len = backing->ByteLength();
  }
  int off = 0;
  if (argc > 2) off = Local<Integer>::Cast(args[2])->Value();
  const char* source = data + off;
  if (!needTranslation(source, len)) {
    args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), source, 
      NewStringType::kNormal, len).ToLocalChecked());
    return;
  }
  std::string escaped = htmlTranslate(source, len);
  args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), 
    escaped.c_str(), NewStringType::kNormal, 
    escaped.length()).ToLocalChecked());
}

void just::html::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> html = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, html, "escape", Escape);
  SET_MODULE(isolate, target, "html", html);
}
