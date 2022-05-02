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
  //Local<ArrayBuffer> kb = args[1].As<ArrayBuffer>();
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
  BIO *publicBIO = BIO_new(BIO_s_mem());
  // dump key to IO
  PEM_write_bio_PUBKEY(publicBIO, key);
  // get buffer length
  int publicKeyLen = BIO_pending(publicBIO);
  // create char reference of public key length
  unsigned char *publicKeyChar = (unsigned char *) calloc(1, publicKeyLen);
  // read the key from the buffer and put it in the char reference
  BIO_read(publicBIO, publicKeyChar, publicKeyLen);
  // at this point we can save the public somewhere
  std::shared_ptr<BackingStore> backing = ArrayBuffer::NewBackingStore(publicKeyChar, publicKeyLen, 
      just::FreeMemory, nullptr);
  Local<ArrayBuffer> kb =
      ArrayBuffer::New(isolate, std::move(backing));
  kb->SetAlignedPointerInInternalField(1, rsa);
  args.GetReturnValue().Set(kb);
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

// https://www.openssl.org/docs/manmaster/man7/X25519.html
void just::signing::CreateKeypair(const FunctionCallbackInfo<Value> &args) {
  // https://gist.github.com/grejdi/9361828
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  EVP_PKEY_CTX *keyCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
  EVP_PKEY_keygen_init(keyCtx);
  EVP_PKEY_CTX_set_rsa_keygen_bits(keyCtx, 2048); // RSA 2048
  // variable that will hold both private and public keys
  EVP_PKEY *key = NULL;
  // generate key
  EVP_PKEY_keygen(keyCtx, &key);
  // free up key context
  EVP_PKEY_CTX_free(keyCtx);

  // extract private key as string
  // create a place to dump the IO, in this case in memory
  BIO *privateBIO = BIO_new(BIO_s_mem());
  // dump key to IO
  PEM_write_bio_PrivateKey(privateBIO, key, NULL, NULL, 0, 0, NULL);
  // get buffer length
  int privateKeyLen = BIO_pending(privateBIO);
  // create char reference of private key length
  unsigned char *privateKeyChar = (unsigned char *) calloc(1, privateKeyLen);
  // read the key from the buffer and put it in the char reference
  BIO_read(privateBIO, privateKeyChar, privateKeyLen);

  // extract public key as string
  // create a place to dump the IO, in this case in memory
  BIO *publicBIO = BIO_new(BIO_s_mem());
  // dump key to IO
  PEM_write_bio_PUBKEY(publicBIO, key);
  // get buffer length
  int publicKeyLen = BIO_pending(publicBIO);
  // create char reference of public key length
  unsigned char *publicKeyChar = (unsigned char *) calloc(1, publicKeyLen);
  // read the key from the buffer and put it in the char reference
  BIO_read(publicBIO, publicKeyChar, publicKeyLen);
  // at this point we can save the public somewhere

  std::shared_ptr<BackingStore> backing1 = ArrayBuffer::NewBackingStore(privateKeyChar, privateKeyLen, 
      just::FreeMemory, nullptr);
  Local<ArrayBuffer> privkb =
      ArrayBuffer::New(isolate, std::move(backing1));
  std::shared_ptr<BackingStore> backing2 = ArrayBuffer::NewBackingStore(publicKeyChar, publicKeyLen, 
      just::FreeMemory, nullptr);
  Local<ArrayBuffer> pubkb =
      ArrayBuffer::New(isolate, std::move(backing2));

  Local<Object> keys = Object::New(isolate);
  keys->Set(context, String::NewFromUtf8Literal(isolate, "private", 
    NewStringType::kInternalized), 
    privkb).Check();
  keys->Set(context, String::NewFromUtf8Literal(isolate, "public", 
    NewStringType::kInternalized), 
    pubkb).Check();
  args.GetReturnValue().Set(keys);
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

void just::signing::GenerateCSR(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> privBuf = args[0].As<ArrayBuffer>();
  RSA *privRSA = (RSA*)privBuf->GetAlignedPointerFromInternalField(1);
  Local<ArrayBuffer> pubBuf = args[1].As<ArrayBuffer>();
  RSA *pubRSA = (RSA*)pubBuf->GetAlignedPointerFromInternalField(1);
  String::Utf8Value hostname(isolate, args[2]);
  String::Utf8Value country(isolate, args[3]);
  String::Utf8Value province(isolate, args[4]);
  String::Utf8Value city(isolate, args[5]);
  String::Utf8Value org(isolate, args[6]);

  const char      *szCommon = *hostname;
  const char      *szCountry = *country;
  const char      *szProvince = *province;
  const char      *szCity = *city;
  const char      *szOrganization = *org;

  X509_REQ* x509_req = X509_REQ_new();
  int r = X509_REQ_set_version(x509_req, 0);
  X509_NAME* x509_name = X509_REQ_get_subject_name(x509_req);
  r = X509_NAME_add_entry_by_txt(x509_name, "C", MBSTRING_ASC, (const unsigned char*)szCountry, -1, -1, 0);
  r = X509_NAME_add_entry_by_txt(x509_name, "ST", MBSTRING_ASC, (const unsigned char*)szProvince, -1, -1, 0);
  r = X509_NAME_add_entry_by_txt(x509_name, "L", MBSTRING_ASC, (const unsigned char*)szCity, -1, -1, 0);
  r = X509_NAME_add_entry_by_txt(x509_name, "O", MBSTRING_ASC, (const unsigned char*)szOrganization, -1, -1, 0);
  r = X509_NAME_add_entry_by_txt(x509_name, "CN", MBSTRING_ASC, (const unsigned char*)szCommon, -1, -1, 0);

  EVP_PKEY* privKey = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(privKey, privRSA);
  EVP_PKEY* pubKey = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(pubKey, pubRSA);

  r = X509_REQ_set_pubkey(x509_req, pubKey);
  r = X509_REQ_sign(x509_req, privKey, EVP_sha1());    // return x509_req->signature->length

  BIO *pemBIO = BIO_new(BIO_s_mem());
  // dump key to IO
  PEM_write_bio_X509_REQ(pemBIO, x509_req);
  // get buffer length
  int pemLen = BIO_pending(pemBIO);
  // create char reference of private key length
  unsigned char *pem = (unsigned char *) calloc(1, pemLen);
  // read the key from the buffer and put it in the char reference
  BIO_read(pemBIO, pem, pemLen);

  std::shared_ptr<BackingStore> pemback = ArrayBuffer::NewBackingStore(pem, pemLen, 
      just::FreeMemory, nullptr);
  Local<ArrayBuffer> ab =
      ArrayBuffer::New(isolate, std::move(pemback));

  args.GetReturnValue().Set(ab);
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
  Local<ObjectTemplate> rsa = ObjectTemplate::New(isolate);
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  SET_VALUE(isolate, module, "version", String::NewFromUtf8Literal(isolate, 
    OPENSSL_VERSION_TEXT));
  SET_METHOD(isolate, rsa, "createKeypair", CreateKeypair);
  SET_METHOD(isolate, rsa, "loadPublicKey", LoadPublicKey);
  SET_METHOD(isolate, rsa, "loadPrivateKey", LoadPrivateKey);
  SET_METHOD(isolate, rsa, "loadCertificate", LoadCertificate);
  SET_METHOD(isolate, rsa, "generateCSR", GenerateCSR);
  SET_METHOD(isolate, rsa, "extractRSAPublicKey", ExtractRSAPublicKey);
  SET_METHOD(isolate, rsa, "sign", Sign);
  SET_METHOD(isolate, rsa, "verify", Verify);
  //encrypt
  //decrypt

  SET_MODULE(isolate, module, "RSA", rsa);
  SET_MODULE(isolate, target, "signing", module);
}
