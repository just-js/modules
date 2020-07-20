#ifndef JUST_ZLIB_H
#define JUST_ZLIB_H

#include "just.h"
#include <zconf.h>
#include <zlib.h>

namespace just {

namespace zlib {

#define Z_MIN_CHUNK 64
#define Z_MAX_CHUNK std::numeric_limits<double>::infinity()
#define Z_DEFAULT_CHUNK (16 * 1024)
#define Z_MIN_MEMLEVEL 1
#define Z_MAX_MEMLEVEL 9
#define Z_DEFAULT_MEMLEVEL 8
#define Z_MIN_LEVEL -1
#define Z_MAX_LEVEL 9
#define Z_DEFAULT_LEVEL Z_DEFAULT_COMPRESSION
#define Z_MIN_WINDOWBITS 8
#define Z_MAX_WINDOWBITS 15
#define Z_DEFAULT_WINDOW_BITS 15

enum zlib_mode {
  NONE,
  DEFLATE,
  INFLATE,
  GZIP,
  GUNZIP,
  DEFLATERAW,
  INFLATERAW,
  UNZIP
};

void FreeMemory(void* buf, size_t length, void* data);
void Crc32(const FunctionCallbackInfo<Value> &args);
void WriteDeflate(const FunctionCallbackInfo<Value> &args);
void WriteInflate(const FunctionCallbackInfo<Value> &args);
void EndDeflate(const FunctionCallbackInfo<Value> &args);
void EndInflate(const FunctionCallbackInfo<Value> &args);
void CreateDeflate(const FunctionCallbackInfo<Value> &args);
void CreateInflate(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_zlib() {
		return (void*)just::zlib::Init;
	}
}
#endif
