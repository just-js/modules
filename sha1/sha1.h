#ifndef JUST_SHA1_H
#define JUST_SHA1_H

#include <just.h>

namespace just {

namespace sha1 {
inline unsigned int rol(const unsigned int value, const unsigned int steps)
{
  return ((value << steps) | (value >> (32 - steps)));
}

inline void clearWBuffert(unsigned int* buffert)
{
  int pos = 0;
  for (pos = 16; --pos >= 0;)
  {
    buffert[pos] = 0;
  }
}

void innerHash(unsigned int* result, unsigned int* w);
void shacalc(const char* src, char* hash, int bytelength);
void Hash(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_sha1() {
		return (void*)just::sha1::Init;
	}
}

#endif
