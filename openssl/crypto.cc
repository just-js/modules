#include "crypto.h"

enum errors {
  SHORT_ARGS = -1,
  BAD_ARGS = -2,
  BAD_DIGEST = -3,
  NO_MEM = -4,
  INIT_FAIL = -5,
  DIGEST_FAIL = -6
};

void just::crypto::Create(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int argc = args.Length();
  if (argc < 3) {
    args.GetReturnValue().Set(Integer::New(isolate, SHORT_ARGS));
    return;
  }
  if (!(args[0]->IsNumber() && args[1]->IsArrayBuffer() && args[2]->IsArrayBuffer())) {
    args.GetReturnValue().Set(Integer::New(isolate, BAD_ARGS));
    return;
  }
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int digest = args[0]->Uint32Value(context).ToChecked();
  const EVP_MD* md = EVP_get_digestbynid(digest);
  if (md == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, BAD_DIGEST));
    return;
  }
  Local<ArrayBuffer> srcab = args[1].As<ArrayBuffer>();
  Local<ArrayBuffer> destab = args[2].As<ArrayBuffer>();
  just::crypto::hasher* hasher = new just::crypto::hasher();
  std::shared_ptr<BackingStore> backing = srcab->GetBackingStore();
  hasher->src = backing->Data();
  hasher->src_len = backing->ByteLength();
  backing = destab->GetBackingStore();
  hasher->dest = backing->Data();
  hasher->md = md;
  hasher->dest_len = backing->ByteLength();
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  if (ctx == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, NO_MEM));
    return;
  }
  int r = EVP_DigestInit_ex(ctx, md, nullptr);
  if (r != 1) {
    args.GetReturnValue().Set(Integer::New(isolate, INIT_FAIL));
    return;
  }
  hasher->ctx = ctx;
  just::crypto::hashers[just::crypto::hcount] = hasher;
  args.GetReturnValue().Set(Integer::New(isolate, just::crypto::hcount++));
}

void just::crypto::Update(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int argc = args.Length();
  if (argc < 1) {
    args.GetReturnValue().Set(Integer::New(isolate, SHORT_ARGS));
    return;
  }
  if (!(args[0]->IsNumber())) {
    args.GetReturnValue().Set(Integer::New(isolate, BAD_ARGS));
    return;
  }
  just::crypto::hasher* hasher = just::crypto::hashers[Local<Integer>::Cast(args[0])->Value()];
  int len = hasher->src_len;
  if (argc > 1) {
    if (!(args[1]->IsNumber())) {
      args.GetReturnValue().Set(Integer::New(isolate, BAD_ARGS));
      return;
    }
    len = Local<Integer>::Cast(args[1])->Value();
  }
  int r = EVP_DigestUpdate(hasher->ctx, reinterpret_cast<const unsigned char*>(
      hasher->src), len);
  if (r != 1) {
    args.GetReturnValue().Set(Integer::New(isolate, DIGEST_FAIL));
    return;
  }
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::crypto::Digest(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int argc = args.Length();
  if (argc < 1) {
    args.GetReturnValue().Set(Integer::New(isolate, SHORT_ARGS));
    return;
  }
  if (!(args[0]->IsNumber())) {
    args.GetReturnValue().Set(Integer::New(isolate, BAD_ARGS));
    return;
  }
  just::crypto::hasher* hasher = just::crypto::hashers[Local<Integer>::Cast(args[0])->Value()];
  unsigned int outlen = 0;
  int r = EVP_DigestFinal(hasher->ctx, (unsigned char*)hasher->dest, &outlen);
  if (r != 1) {
    args.GetReturnValue().Set(Integer::New(isolate, DIGEST_FAIL));
    return;
  }
  EVP_MD_CTX_reset(hasher->ctx);
  r = EVP_DigestInit_ex(hasher->ctx, hasher->md, nullptr);
  args.GetReturnValue().Set(Integer::New(isolate, outlen));
}

void just::crypto::Reset(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int argc = args.Length();
  if (argc < 1) {
    args.GetReturnValue().Set(Integer::New(isolate, SHORT_ARGS));
    return;
  }
  if (!(args[0]->IsNumber())) {
    args.GetReturnValue().Set(Integer::New(isolate, BAD_ARGS));
    return;
  }
  just::crypto::hasher* hasher = just::crypto::hashers[Local<Integer>::Cast(args[0])->Value()];
  int r = EVP_MD_CTX_reset(hasher->ctx);
  if (r != 1) {
    args.GetReturnValue().Set(Integer::New(isolate, DIGEST_FAIL));
    return;
  }
  r = EVP_DigestInit_ex(hasher->ctx, hasher->md, nullptr);
  if (r != 1) {
    args.GetReturnValue().Set(Integer::New(isolate, DIGEST_FAIL));
    return;
  }
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::crypto::Free(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int argc = args.Length();
  if (argc < 1) {
    args.GetReturnValue().Set(Integer::New(isolate, SHORT_ARGS));
    return;
  }
  if (!(args[0]->IsNumber())) {
    args.GetReturnValue().Set(Integer::New(isolate, BAD_ARGS));
    return;
  }
  just::crypto::hasher* hasher = just::crypto::hashers[Local<Integer>::Cast(args[0])->Value()];
  EVP_MD_CTX_free(hasher->ctx);
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::crypto::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  if (!just::crypto::initialized) {
    EVP_add_digest(EVP_md5());
    EVP_add_digest(EVP_sha1());
    EVP_add_digest(EVP_sha256());
    EVP_add_digest(EVP_sha512());
    EVP_add_digest(EVP_sha224());
    EVP_add_digest(EVP_md4());
    EVP_add_digest(EVP_sha384());
    EVP_add_digest(EVP_ripemd160());
    just::crypto::initialized = true;
  }

  SET_VALUE(isolate, module, "version", String::NewFromUtf8Literal(isolate, 
    OPENSSL_VERSION_TEXT));
  SET_VALUE(isolate, module, "MD5", Integer::New(isolate, 
    OBJ_txt2nid("md5")));
  SET_VALUE(isolate, module, "SHA1", Integer::New(isolate, 
    OBJ_txt2nid("sha1")));
  SET_VALUE(isolate, module, "SHA256", Integer::New(isolate, 
    OBJ_txt2nid("sha256")));
  SET_VALUE(isolate, module, "SHA512", Integer::New(isolate, 
    OBJ_txt2nid("sha512")));
  SET_VALUE(isolate, module, "MD4", Integer::New(isolate, 
    OBJ_txt2nid("md4")));
  SET_VALUE(isolate, module, "SHA224", Integer::New(isolate, 
    OBJ_txt2nid("sha224")));
  SET_VALUE(isolate, module, "SHA384", Integer::New(isolate, 
    OBJ_txt2nid("sha384")));
  SET_VALUE(isolate, module, "RIPEMD160", Integer::New(isolate, 
    OBJ_txt2nid("ripemd160")));

  Local<ObjectTemplate> errors = ObjectTemplate::New(isolate);
  SET_VALUE(isolate, errors, "SHORT_ARGS", Integer::New(isolate, SHORT_ARGS));
  SET_VALUE(isolate, errors, "BAD_ARGS", Integer::New(isolate, BAD_ARGS));
  SET_VALUE(isolate, errors, "BAD_DIGEST", Integer::New(isolate, BAD_DIGEST));
  SET_VALUE(isolate, errors, "NO_MEM", Integer::New(isolate, NO_MEM));
  SET_VALUE(isolate, errors, "INIT_FAIL", Integer::New(isolate, INIT_FAIL));
  SET_VALUE(isolate, errors, "DIGEST_FAIL", Integer::New(isolate, DIGEST_FAIL));
  module->Set(String::NewFromUtf8Literal(isolate, "errors", 
    NewStringType::kNormal), errors);

  SET_METHOD(isolate, module, "create", Create);
  SET_METHOD(isolate, module, "update", Update);
  SET_METHOD(isolate, module, "digest", Digest);
  SET_METHOD(isolate, module, "reset", Reset);
  SET_METHOD(isolate, module, "free", Free);

  SET_MODULE(isolate, target, "crypto", module);
}
