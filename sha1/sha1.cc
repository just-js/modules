#include "sha1.h"

void just::sha1::innerHash(unsigned int* result, unsigned int* w)
{
  unsigned int a = result[0];
  unsigned int b = result[1];
  unsigned int c = result[2];
  unsigned int d = result[3];
  unsigned int e = result[4];
  int round = 0;
#define sha1macro(func,val) \
{ \
const unsigned int t = rol(a, 5) + (func) + e + val + w[round]; \
e = d; \
d = c; \
c = rol(b, 30); \
b = a; \
a = t; \
}
  while (round < 16)
  {
    sha1macro((b & c) | (~b & d), 0x5a827999)
    ++round;
  }
  while (round < 20)
  {
    w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
    sha1macro((b & c) | (~b & d), 0x5a827999)
    ++round;
  }
  while (round < 40)
  {
    w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
    sha1macro(b ^ c ^ d, 0x6ed9eba1)
    ++round;
  }
  while (round < 60)
  {
    w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
    sha1macro((b & c) | (b & d) | (c & d), 0x8f1bbcdc)
    ++round;
  }
  while (round < 80)
  {
    w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
    sha1macro(b ^ c ^ d, 0xca62c1d6)
    ++round;
  }
#undef sha1macro
  result[0] += a;
  result[1] += b;
  result[2] += c;
  result[3] += d;
  result[4] += e;
}

void just::sha1::shacalc(const char* src, char* hash, int bytelength)
{
  unsigned int result[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };
  const unsigned char* sarray = (const unsigned char*) src;
  unsigned int w[80];
  const int endOfFullBlocks = bytelength - 64;
  int endCurrentBlock;
  int currentBlock = 0;
  while (currentBlock <= endOfFullBlocks)
  {
    endCurrentBlock = currentBlock + 64;
    int roundPos = 0;
    for (roundPos = 0; currentBlock < endCurrentBlock; currentBlock += 4)
    {
      w[roundPos++] = (unsigned int) sarray[currentBlock + 3]
        | (((unsigned int) sarray[currentBlock + 2]) << 8)
        | (((unsigned int) sarray[currentBlock + 1]) << 16)
        | (((unsigned int) sarray[currentBlock]) << 24);
    }
    innerHash(result, w);
  }
  endCurrentBlock = bytelength - currentBlock;
  clearWBuffert(w);
  int lastBlockBytes = 0;
  for (;lastBlockBytes < endCurrentBlock; ++lastBlockBytes)
  {
    w[lastBlockBytes >> 2] |= (unsigned int) sarray[lastBlockBytes + currentBlock] << ((3 - (lastBlockBytes & 3)) << 3);
  }
  w[lastBlockBytes >> 2] |= 0x80 << ((3 - (lastBlockBytes & 3)) << 3);
  if (endCurrentBlock >= 56)
  {
    innerHash(result, w);
    clearWBuffert(w);
  }
  w[15] = bytelength << 3;
  innerHash(result, w);
  int hashByte = 0;
  for (hashByte = 20; --hashByte >= 0;)
  {
    hash[hashByte] = (result[hashByte >> 2] >> (((3 - hashByte) & 0x3) << 3)) & 0xff;
  }
}

void just::sha1::Hash(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> absource = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> source = absource->GetBackingStore();
  Local<ArrayBuffer> abdest = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> dest = abdest->GetBackingStore();
  int len = source->ByteLength();
  if (args.Length() > 3) {
    len = args[3]->Uint32Value(context).ToChecked();
  }
  shacalc((const char*)source->Data(), (char*)dest->Data(), len);
  args.GetReturnValue().Set(Integer::New(isolate, 20));
}

void just::sha1::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "hash", Hash);
  SET_MODULE(isolate, target, "sha1", module);
}
