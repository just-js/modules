#ifndef JUST_FIB_H
#define JUST_FIB_H

#include <just.h>
#include <v8-profiler.h>

namespace just {

namespace profiler {

class FileOutputStream : public v8::OutputStream {
 public:
  explicit FileOutputStream(FILE* stream) : stream_(stream) {}

  virtual int GetChunkSize() {
    return 65536;  // big chunks == faster
  }

  virtual void EndOfStream() {}

  virtual WriteResult WriteAsciiChunk(char* data, int size) {
    const size_t len = static_cast<size_t>(size);
    size_t off = 0;

    while (off < len && !feof(stream_) && !ferror(stream_))
      off += fwrite(data + off, 1, len - off, stream_);

    return off == len ? kContinue : kAbort;
  }

 private:
  FILE* stream_;
};

void Snapshot(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_profiler() {
		return (void*)just::profiler::Init;
	}
}

#endif
