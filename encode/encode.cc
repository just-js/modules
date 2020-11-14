#include "encode.h"

size_t just::encode::hex_decode(char* buf,
                         size_t len,
                         const char* src,
                         const size_t srcLen) {
  size_t i;
  for (i = 0; i < len && i * 2 + 1 < srcLen; ++i) {
    unsigned a = unhex(src[i * 2 + 0]);
    unsigned b = unhex(src[i * 2 + 1]);
    if (!~a || !~b)
      return i;
    buf[i] = (a << 4) | b;
  }

  return i;
}

size_t just::encode::base64_decoded_size(const char* src, size_t size) {
  if (size == 0)
    return 0;

  if (src[size - 1] == '=')
    size--;
  if (size > 0 && src[size - 1] == '=')
    size--;

  return base64_decoded_size_fast(size);
}

bool just::encode::base64_decode_group_slow(char* dst, const size_t dstlen,
                              const char* src, const size_t srclen,
                              size_t* const i, size_t* const k) {
  uint8_t hi;
  uint8_t lo;
#define V(expr)                                                               \
  for (;;) {                                                                  \
    const uint8_t c = src[*i];                                                \
    lo = unbase64(c);                                                         \
    *i += 1;                                                                  \
    if (lo < 64)                                                              \
      break;  /* Legal character. */                                          \
    if (c == '=' || *i >= srclen)                                             \
      return false;  /* Stop decoding. */                                     \
  }                                                                           \
  expr;                                                                       \
  if (*i >= srclen)                                                           \
    return false;                                                             \
  if (*k >= dstlen)                                                           \
    return false;                                                             \
  hi = lo;
  V(/* Nothing. */);
  V(dst[(*k)++] = ((hi & 0x3F) << 2) | ((lo & 0x30) >> 4));
  V(dst[(*k)++] = ((hi & 0x0F) << 4) | ((lo & 0x3C) >> 2));
  V(dst[(*k)++] = ((hi & 0x03) << 6) | ((lo & 0x3F) >> 0));
#undef V
  return true;  // Continue decoding.
}

size_t just::encode::base64_decode_fast(char* dst, const size_t dstlen,
                          const char* src, const size_t srclen,
                          const size_t decoded_size) {
  const size_t available = dstlen < decoded_size ? dstlen : decoded_size;
  const size_t max_k = available / 3 * 3;
  size_t max_i = srclen / 4 * 4;
  size_t i = 0;
  size_t k = 0;
  while (i < max_i && k < max_k) {
    const uint32_t v =
        unbase64(src[i + 0]) << 24 |
        unbase64(src[i + 1]) << 16 |
        unbase64(src[i + 2]) << 8 |
        unbase64(src[i + 3]);
    // If MSB is set, input contains whitespace or is not valid base64.
    if (v & 0x80808080) {
      if (!base64_decode_group_slow(dst, dstlen, src, srclen, &i, &k))
        return k;
      max_i = i + (srclen - i) / 4 * 4;  // Align max_i again.
    } else {
      dst[k + 0] = ((v >> 22) & 0xFC) | ((v >> 20) & 0x03);
      dst[k + 1] = ((v >> 12) & 0xF0) | ((v >> 10) & 0x0F);
      dst[k + 2] = ((v >>  2) & 0xC0) | ((v >>  0) & 0x3F);
      i += 4;
      k += 3;
    }
  }
  if (i < srclen && k < dstlen) {
    base64_decode_group_slow(dst, dstlen, src, srclen, &i, &k);
  }
  return k;
}

size_t just::encode::base64_decode(char* dst, const size_t dstlen,
                     const char* src, const size_t srclen) {
  const size_t decoded_size = base64_decoded_size(src, srclen);
  return base64_decode_fast(dst, dstlen, src, srclen, decoded_size);
}

size_t just::encode::base64_encode(const char* src,
                            size_t slen,
                            char* dst,
                            size_t dlen) {
  // We know how much we'll write, just make sure that there's space.
  dlen = base64_encoded_size(slen);

  unsigned a;
  unsigned b;
  unsigned c;
  unsigned i;
  unsigned k;
  unsigned n;

  static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz"
                              "0123456789+/";

  i = 0;
  k = 0;
  n = slen / 3 * 3;

  while (i < n) {
    a = src[i + 0] & 0xff;
    b = src[i + 1] & 0xff;
    c = src[i + 2] & 0xff;

    dst[k + 0] = table[a >> 2];
    dst[k + 1] = table[((a & 3) << 4) | (b >> 4)];
    dst[k + 2] = table[((b & 0x0f) << 2) | (c >> 6)];
    dst[k + 3] = table[c & 0x3f];

    i += 3;
    k += 4;
  }

  if (n != slen) {
    switch (slen - n) {
      case 1:
        a = src[i + 0] & 0xff;
        dst[k + 0] = table[a >> 2];
        dst[k + 1] = table[(a & 3) << 4];
        dst[k + 2] = '=';
        dst[k + 3] = '=';
        break;

      case 2:
        a = src[i + 0] & 0xff;
        b = src[i + 1] & 0xff;
        dst[k + 0] = table[a >> 2];
        dst[k + 1] = table[((a & 3) << 4) | (b >> 4)];
        dst[k + 2] = table[(b & 0x0f) << 2];
        dst[k + 3] = '=';
        break;
    }
  }

  return dlen;
}

void just::encode::HexEncode(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> absource = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> source = absource->GetBackingStore();
  Local<ArrayBuffer> abdest = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> dest = abdest->GetBackingStore();
  int len = source->ByteLength();
  if (args.Length() > 2) {
    len = args[2]->Uint32Value(context).ToChecked();
  }
  int off = 0;
  if (args.Length() > 3) {
    off = args[3]->Uint32Value(context).ToChecked();
  }
  char* dst = (char*)dest->Data() + off;
  size_t bytes = hex_encode((const char*)source->Data(), len, 
    dst, dest->ByteLength() - off);
  args.GetReturnValue().Set(Integer::New(isolate, bytes));
}

void just::encode::Base64Encode(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> absource = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> source = absource->GetBackingStore();
  Local<ArrayBuffer> abdest = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> dest = abdest->GetBackingStore();
  int len = source->ByteLength();
  if (args.Length() > 2) {
    len = args[2]->Uint32Value(context).ToChecked();
  }
  size_t dlen = base64_encoded_size(len);
  size_t bytes = base64_encode((const char*)source->Data(), len, 
    (char*)dest->Data(), dlen);
  args.GetReturnValue().Set(Integer::New(isolate, bytes));
}

void just::encode::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "hexEncode", HexEncode);
  SET_METHOD(isolate, module, "base64Encode", Base64Encode);
  SET_MODULE(isolate, target, "encode", module);
}
