#ifndef JUST_HTTP_H
#define JUST_HTTP_H

#include "just.h"
#include <hescape.h>

namespace just {

namespace html {

class EscapedString
    : public v8::String::ExternalOneByteStringResource {
 public:
  explicit EscapedString(const uint8_t* data, size_t length)
      : data_(data), length_(length) {}
  ~EscapedString() {
		delete data_;
	}

  const char* data() const override {
    return reinterpret_cast<const char*>(data_);
  }
  size_t length() const override { return length_; }

  EscapedString(const EscapedString&) =
      delete;
  EscapedString& operator=(
      const EscapedString&) = delete;

 private:
  const uint8_t* data_;
  size_t length_;
};

void Escape(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_html() {
		return (void*)just::html::Init;
	}
}
#endif
