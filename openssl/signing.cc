#include "signing.h"

// https://www.openssl.org/docs/manmaster/man3/PEM_read_bio_RSA_PUBKEY.html
void just::signing::LoadPublicKey(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> pk = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = pk->GetBackingStore();
  const void* data = backing->Data();
  RSA *rsa = NULL;
  BIO *keybio = BIO_new_mem_buf(data, -1);
  if (keybio == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
    return;
  }
  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
  if (rsa == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -2));
    return;
  }
  pk->SetAlignedPointerInInternalField(1, rsa);
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::signing::ExtractRSAPublicKey(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> cb = args[0].As<ArrayBuffer>();
  Local<ArrayBuffer> kb = args[1].As<ArrayBuffer>();
  //std::shared_ptr<BackingStore> cbb = cb->GetBackingStore();
  X509 *cert = (X509*)cb->GetAlignedPointerFromInternalField(1);
  //const void* data = cbb->Data();
  EVP_PKEY* key = X509_get_pubkey(cert);
  if (key == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
    return;
  }
  int idKey = EVP_PKEY_id(key);
  int type = EVP_PKEY_type(idKey);
  if (type != EVP_PKEY_RSA && type != EVP_PKEY_RSA2) {
    args.GetReturnValue().Set(Integer::New(isolate, -2));
    return;
  }
  RSA* rsa = EVP_PKEY_get1_RSA(key);
  if (rsa == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -3));
    return;
  }
  kb->SetAlignedPointerInInternalField(1, rsa);
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::signing::LoadCertificate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> pk = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = pk->GetBackingStore();
  const void* data = backing->Data();
  // TODO: free these BIOs
  BIO *keybio = BIO_new_mem_buf(data, -1);
  if (keybio == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
    return;
  }
  X509* cert = PEM_read_bio_X509(keybio, nullptr, NULL, NULL);
  if (cert == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -2));
    return;
  }
  pk->SetAlignedPointerInInternalField(1, cert);
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::signing::Verify(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> pk = args[0].As<ArrayBuffer>();
  RSA *rsa = (RSA*)pk->GetAlignedPointerFromInternalField(1);
  Local<ArrayBuffer> pt = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = pt->GetBackingStore();
  const void* plaintext = backing->Data();
  size_t pt_len = backing->ByteLength();
  Local<ArrayBuffer> sig = args[2].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing2 = sig->GetBackingStore();
  const unsigned char* signature = (const unsigned char*)backing2->Data();
  size_t sig_len = backing2->ByteLength();
  EVP_PKEY* pubKey  = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(pubKey, rsa);
  EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();
  const EVP_MD* digest = EVP_sha512();
  if (args.Length() > 3) {
    digest = EVP_get_digestbynid(Local<Integer>::Cast(args[3])->Value());
  }
  if (EVP_DigestVerifyInit(m_RSAVerifyCtx, NULL, digest, NULL, pubKey) <= 0) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
    EVP_MD_CTX_free(m_RSAVerifyCtx);
    return;
  }
  if (EVP_DigestVerifyUpdate(m_RSAVerifyCtx, plaintext, pt_len) <= 0) {
    args.GetReturnValue().Set(Integer::New(isolate, -2));
    EVP_MD_CTX_free(m_RSAVerifyCtx);
    return;
  }
  int rc = EVP_DigestVerifyFinal(m_RSAVerifyCtx, signature, sig_len);
  EVP_MD_CTX_free(m_RSAVerifyCtx);
  if (rc != 1) {
    args.GetReturnValue().Set(Integer::New(isolate, -2));
    return;
  }
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::signing::LoadPrivateKey(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> pk = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = pk->GetBackingStore();
  const void* data = backing->Data();
  RSA *rsa = NULL;
  BIO *keybio = BIO_new_mem_buf(data, -1);
  if (keybio == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
    return;
  }
  rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
  if (rsa == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
    return;
  }
  pk->SetAlignedPointerInInternalField(1, rsa);
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::signing::Sign(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> pk = args[0].As<ArrayBuffer>();
  RSA *rsa = (RSA*)pk->GetAlignedPointerFromInternalField(1);
  Local<ArrayBuffer> pt = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = pt->GetBackingStore();
  const void* plaintext = backing->Data();
  size_t pt_len = backing->ByteLength();
  Local<ArrayBuffer> sig = args[2].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing2 = sig->GetBackingStore();
  unsigned char* signature = (unsigned char*)backing2->Data();
  //size_t sig_len = backing2->ByteLength();
  EVP_PKEY* privKey  = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(privKey, rsa);
  EVP_MD_CTX* m_RSASignCtx = EVP_MD_CTX_create();
  const EVP_MD* digest = EVP_sha512();
  if (args.Length() > 3) {
    digest = EVP_get_digestbynid(Local<Integer>::Cast(args[3])->Value());
  }
  if (EVP_DigestSignInit(m_RSASignCtx, NULL, digest, NULL, privKey) <= 0) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
    EVP_MD_CTX_free(m_RSASignCtx);
    return;
  }
  if (EVP_DigestSignUpdate(m_RSASignCtx, plaintext, pt_len) <= 0) {
    args.GetReturnValue().Set(Integer::New(isolate, -2));
    EVP_MD_CTX_free(m_RSASignCtx);
    return;
  }
  size_t MsgLenEnc;
  if (EVP_DigestSignFinal(m_RSASignCtx, NULL, &MsgLenEnc) <= 0) {
    args.GetReturnValue().Set(Integer::New(isolate, -3));
    EVP_MD_CTX_free(m_RSASignCtx);
    return;
  }
  int rc = EVP_DigestSignFinal(m_RSASignCtx, signature, &MsgLenEnc);
  EVP_MD_CTX_free(m_RSASignCtx);
  if (rc <= 0) {
    args.GetReturnValue().Set(Integer::New(isolate, -4));
    return;
  }
  args.GetReturnValue().Set(Integer::New(isolate, MsgLenEnc));
}

void just::signing::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  SET_VALUE(isolate, module, "version", String::NewFromUtf8Literal(isolate, 
    OPENSSL_VERSION_TEXT));
  SET_METHOD(isolate, module, "loadPublicKey", LoadPublicKey);
  SET_METHOD(isolate, module, "loadPrivateKey", LoadPrivateKey);
  SET_METHOD(isolate, module, "loadCertificate", LoadCertificate);
  SET_METHOD(isolate, module, "extractRSAPublicKey", ExtractRSAPublicKey);
  SET_METHOD(isolate, module, "sign", Sign);
  SET_METHOD(isolate, module, "verify", Verify);

  SET_MODULE(isolate, target, "signing", module);
}
