#include "lz4.h"

void just::lz4::CompressBound(const FunctionCallbackInfo<Value> &args) {
/*
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  unsigned int len = args[1]->Uint32Value(context).ToChecked();
  bool lossless = true;
  Local<BigInt> crc64 = args[2]->ToBigInt(context).ToLocalChecked();
  uint64_t crc = crc64->Uint64Value(&lossless);
  unsigned int off = 0;
  if (args.Length() > 3) {
    off = args[3]->Uint32Value(context).ToChecked();
  }
  const uint8_t* source = (const uint8_t *)backing->Data() + off;
  args.GetReturnValue().Set(BigInt::New(isolate, crc32(crc, source, len)));
  // LZ4_compressBound 
*/
}

void just::lz4::Compress(const FunctionCallbackInfo<Value> &args) {
  // LZ4_compress_default
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  const int max_dst_size = LZ4_compressBound(ab->ByteLength());
  Local<ArrayBuffer> dest = ArrayBuffer::New(isolate, max_dst_size);

/*
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  z_stream* stream = (z_stream*)ab->GetAlignedPointerFromInternalField(1);
  int argc = args.Length();
  unsigned int flush = Z_NO_FLUSH;
  unsigned int len = args[1]->Uint32Value(context).ToChecked();
  if (argc > 2) {
    flush = args[2]->Uint32Value(context).ToChecked();
  }
  unsigned char* next_in = stream->next_in;
  unsigned int avail_in = stream->avail_in;
  unsigned char* next_out = stream->next_out;
  unsigned int avail_out = stream->avail_out;
  stream->avail_in = len;
  int err = deflate(stream, flush);
  if (err < 0) {
    args.GetReturnValue().Set(Integer::New(isolate, err));
    return;
  }
  unsigned int bytes = avail_out - stream->avail_out;
  stream->next_in = next_in;
  stream->avail_in = avail_in;
  stream->next_out = next_out;
  stream->avail_out = avail_out;
  args.GetReturnValue().Set(Integer::New(isolate, bytes));
*/
}

void just::lz4::Decompress(const FunctionCallbackInfo<Value> &args) {
  //LZ4_decompress_safe
/*
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> outab = args[0].As<ArrayBuffer>();
  z_stream* stream = (z_stream*)outab->GetAlignedPointerFromInternalField(1);
  Local<ArrayBuffer> inab = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> in = inab->GetBackingStore();
  unsigned int flush = Z_NO_FLUSH;
  unsigned int off = args[2]->Uint32Value(context).ToChecked();
  unsigned int len = args[3]->Uint32Value(context).ToChecked();
  Local<Array> state = args[4].As<Array>();
  int argc = args.Length();
  if (argc > 5) {
    flush = args[5]->Uint32Value(context).ToChecked();
  }
  stream->next_in = (unsigned char*)in->Data() + off;
  stream->avail_in = len;
  unsigned int avail_in = stream->avail_in;
  unsigned char* next_out = stream->next_out;
  unsigned int avail_out = stream->avail_out;
  int err = inflate(stream, flush);
  unsigned int byteswritten = avail_out - stream->avail_out;
  unsigned int bytesread = avail_in - stream->avail_in;
  stream->next_out = next_out;
  stream->avail_out = avail_out;
  state->Set(context, 0, Integer::New(isolate, bytesread)).Check();
  state->Set(context, 1, Integer::New(isolate, byteswritten)).Check();
  args.GetReturnValue().Set(Integer::New(isolate, err));
*/
}

void just::lz4::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "compressBound", CompressBound);
  SET_METHOD(isolate, module, "compress", Compress);
  SET_METHOD(isolate, module, "decompress", Decompress);
/*
  SET_VALUE(isolate, module, "Z_NO_FLUSH", Integer::New(isolate, 
    Z_NO_FLUSH));
  SET_VALUE(isolate, module, "Z_FULL_FLUSH", Integer::New(isolate, 
    Z_FULL_FLUSH));
  SET_VALUE(isolate, module, "Z_FINISH", Integer::New(isolate, Z_FINISH));
  SET_VALUE(isolate, module, "Z_DEFAULT_COMPRESSION", Integer::New(isolate, 
    Z_DEFAULT_COMPRESSION));
  SET_VALUE(isolate, module, "Z_BEST_COMPRESSION", Integer::New(isolate, 
    Z_BEST_COMPRESSION));
  SET_VALUE(isolate, module, "Z_NO_COMPRESSION", Integer::New(isolate, 
    Z_NO_COMPRESSION));
  SET_VALUE(isolate, module, "Z_BEST_SPEED", Integer::New(isolate, 
    Z_BEST_SPEED));
  SET_VALUE(isolate, module, "Z_DEFAULT_WINDOW_BITS", Integer::New(isolate, 
    Z_DEFAULT_WINDOW_BITS));
  SET_VALUE(isolate, module, "Z_OK", Integer::New(isolate, Z_OK));
  SET_VALUE(isolate, module, "Z_STREAM_END", Integer::New(isolate, 
    Z_STREAM_END));
  SET_VALUE(isolate, module, "Z_NEED_DICT", Integer::New(isolate, Z_NEED_DICT));
  SET_VALUE(isolate, module, "Z_ERRNO", Integer::New(isolate, Z_ERRNO));
  SET_VALUE(isolate, module, "Z_STREAM_ERROR", Integer::New(isolate, 
    Z_STREAM_ERROR));
  SET_VALUE(isolate, module, "Z_DATA_ERROR", Integer::New(isolate, 
    Z_DATA_ERROR));
  SET_VALUE(isolate, module, "Z_MEM_ERROR", Integer::New(isolate, Z_MEM_ERROR));
  SET_VALUE(isolate, module, "Z_BUF_ERROR", Integer::New(isolate, Z_BUF_ERROR));
  SET_VALUE(isolate, module, "Z_VERSION_ERROR", Integer::New(isolate, 
    Z_VERSION_ERROR));
*/
  SET_MODULE(isolate, target, "lz4", module);
}
