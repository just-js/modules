#include "crypto.h"

void just::crypto::Hash(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  unsigned int digest = args[0]->Uint32Value(context).ToChecked();
  Local<ArrayBuffer> absource = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> source = absource->GetBackingStore();
  Local<ArrayBuffer> abdest = args[2].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> dest = abdest->GetBackingStore();
  int len = source->ByteLength();
  unsigned int outlen = 0;
  if (args.Length() > 3) {
    len = args[3]->Uint32Value(context).ToChecked();
  }
  const EVP_MD* md = EVP_get_digestbynid(digest);
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx, md, nullptr);
  EVP_DigestUpdate(ctx, reinterpret_cast<const unsigned char*>(
      source->Data()), len);
  EVP_DigestFinal(ctx, (unsigned char*)dest->Data(), &outlen);
  EVP_MD_CTX_free(ctx);
  args.GetReturnValue().Set(Integer::New(isolate, outlen));
}

void just::crypto::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
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
  SET_METHOD(isolate, module, "hash", Hash);
  SET_MODULE(isolate, target, "crypto", module);
  EVP_add_digest(EVP_md5());
  EVP_add_digest(EVP_sha1());
  EVP_add_digest(EVP_sha256());
  EVP_add_digest(EVP_sha512());
}
