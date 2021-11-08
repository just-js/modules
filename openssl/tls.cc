#include "tls.h"

void just::tls::Error(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  int rc = args[1]->Uint32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, SSL_get_error(ssl, rc)));
}

void just::tls::Shutdown(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  args.GetReturnValue().Set(Integer::New(isolate, SSL_shutdown(ssl)));
}

void just::tls::GetServerName(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  const char* serverName = SSL_get_servername(ssl, SSL_get_servername_type(ssl));
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, serverName, 
    NewStringType::kNormal, strlen(serverName)).ToLocalChecked());
}

void just::tls::Free(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  SSL_free(ssl);
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::tls::Read(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  std::shared_ptr<BackingStore> buf = ab->GetBackingStore();
  int argc = args.Length();
  int len = buf->ByteLength();
  int off = 0;
  if (argc > 1) {
    len = args[1]->Int32Value(context).ToChecked();
  }
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  const char* data = (const char*)buf->Data() + off;
  args.GetReturnValue().Set(Integer::New(isolate, SSL_read(ssl, 
    (void*)data, len)));
}

void just::tls::Write(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  //fprintf(stderr, "ssl %lu", (uint64_t)ssl);
  std::shared_ptr<BackingStore> buf = ab->GetBackingStore();
  int argc = args.Length();
  int len = buf->ByteLength();
  int off = 0;
  if (argc > 1) {
    len = args[1]->Int32Value(context).ToChecked();
  }
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  char* dest = (char*)buf->Data() + off;
  // todo: this is ugly - pick one way of creating a "handle" and use it everywhere
  if (argc > 3) {
    Local<ArrayBuffer> ab = args[3].As<ArrayBuffer>();
    std::shared_ptr<BackingStore> buf = ab->GetBackingStore();
    dest = (char*)buf->Data() + off;
  }
  args.GetReturnValue().Set(Integer::New(isolate, SSL_write(ssl, 
    (void*)dest, len)));
}

void just::tls::GetCertificate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  fprintf(stderr, "server version: %s\n", SSL_get_version(ssl));
  fprintf(stderr, "using cipher %s\n", SSL_get_cipher(ssl));
  X509 *server_cert = SSL_get_peer_certificate(ssl);
  if (server_cert != NULL) {
    char *str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
    if(str == NULL) fprintf(stderr, "warn X509 subject name is null\n");
    fprintf(stderr, "Subject: %s\n", str);
    OPENSSL_free(str);
    str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
    if(str == NULL) fprintf(stderr, "warn X509 issuer name is null\n");
    fprintf(stderr, "Issuer: %s\n", str);
    OPENSSL_free(str);
    X509_free(server_cert);
  } else {
    fprintf(stderr, "server does not have certificate.\n");
  }
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::tls::Handshake(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  args.GetReturnValue().Set(Integer::New(isolate, SSL_do_handshake(ssl)));
}

void just::tls::SetCiphers(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL_CTX* ctx = (SSL_CTX*)ab->GetAlignedPointerFromInternalField(1);
  String::Utf8Value ciphers(isolate, args[1]);
  args.GetReturnValue().Set(Integer::New(isolate, SSL_CTX_set_ciphersuites(ctx, *ciphers)));
}

int just::tls::SelectSNIContextCallback(SSL* ssl, int* ad, void* arg) {
  const char* servername = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
  fprintf(stderr, "%s\n", servername);
  return SSL_TLSEXT_ERR_OK;
}

void just::tls::SetSNICallback(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  SSL_CTX* ctx = (SSL_CTX*)ab->GetAlignedPointerFromInternalField(1);

  SSL_CTX_set_tlsext_servername_callback(ctx, just::tls::SelectSNIContextCallback);

  //String::Utf8Value ciphers(isolate, args[1]);
  //args.GetReturnValue().Set(Integer::New(isolate, SSL_CTX_set_ciphersuites(ctx, *ciphers)));
}

void just::tls::SetContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  ab = args[1].As<ArrayBuffer>();
  SSL_CTX* ctx = (SSL_CTX*)ab->GetAlignedPointerFromInternalField(1);
  SSL_set_SSL_CTX(ssl, ctx);
}

void just::tls::AcceptSocket(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Uint32Value(context).ToChecked();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  // todo: check for empty
  SSL_CTX* ctx = (SSL_CTX*)ab->GetAlignedPointerFromInternalField(1);
  SSL* ssl = SSL_new(ctx);
  SSL_set_fd(ssl, fd);
  SSL_set_accept_state(ssl);
  Local<ArrayBuffer> buf = args[2].As<ArrayBuffer>();
  buf->SetAlignedPointerInInternalField(1, ssl);
  args.GetReturnValue().Set(Integer::New(isolate, SSL_do_handshake(ssl)));
  //args.GetReturnValue().Set(Integer::New(isolate, 1));
}

void just::tls::ConnectSocket(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Uint32Value(context).ToChecked();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  // todo: check for empty
  SSL_CTX* ctx = (SSL_CTX*)ab->GetAlignedPointerFromInternalField(1);
  SSL* ssl = SSL_new(ctx);
  SSL_set_fd(ssl, fd);
  SSL_set_connect_state(ssl);
  Local<ArrayBuffer> buf = args[2].As<ArrayBuffer>();
  buf->SetAlignedPointerInInternalField(1, ssl);
  // todo: should this be SSL_connect(ssl)?
  args.GetReturnValue().Set(Integer::New(isolate, SSL_do_handshake(ssl)));
}

void just::tls::ServerContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  const SSL_METHOD *meth;
  meth = TLS_server_method();
  SSL_CTX* ctx = SSL_CTX_new(meth);
  ab->SetAlignedPointerInInternalField(1, ctx);
  int options = SSL_OP_ALL | SSL_OP_NO_RENEGOTIATION | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_DTLSv1 | SSL_OP_NO_DTLSv1_2;
  int argc = args.Length();
  if (argc > 1) {
    String::Utf8Value cert(isolate, args[1]);
    SSL_CTX_use_certificate_chain_file(ctx, *cert);
  }
  if (argc > 2) {
    String::Utf8Value key(isolate, args[2]);
    SSL_CTX_use_PrivateKey_file(ctx, *key, SSL_FILETYPE_PEM);
  }
  if (argc > 3) {
    Local<BigInt> i64 = Local<BigInt>::Cast(args[3]);
    options = i64->Uint64Value();
  }
  SSL_CTX_set_options(ctx, options);
  if (argc > 4) {
    String::Utf8Value ciphers(isolate, args[4]);
    SSL_CTX_set_cipher_list(ctx, *ciphers);
    SSL_CTX_set_ciphersuites(ctx, *ciphers);
  }
  args.GetReturnValue().Set(ab);
}

void just::tls::ClientContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  const SSL_METHOD *meth;
  meth = TLS_client_method();
  SSL_CTX* ctx = SSL_CTX_new(meth);
  ab->SetAlignedPointerInInternalField(1, ctx);
  int argc = args.Length();
  if (argc > 1) {
    String::Utf8Value cert(isolate, args[1]);
    SSL_CTX_use_certificate_file(ctx, *cert, SSL_FILETYPE_PEM);
  }
  if (argc > 2) {
    String::Utf8Value key(isolate, args[2]);
    SSL_CTX_use_PrivateKey_file(ctx, *key, SSL_FILETYPE_PEM);
  }
  // todo: check private key
  args.GetReturnValue().Set(ab);
}

void just::tls::DestroyContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  // todo: check for empty
  SSL_CTX* ctx = (SSL_CTX*)ab->GetAlignedPointerFromInternalField(1);
  SSL_CTX_free(ctx);
}

void just::tls::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_VALUE(isolate, module, "version", String::NewFromUtf8Literal(isolate, 
    OPENSSL_VERSION_TEXT));
  SET_VALUE(isolate, module, "SSL_ERROR_WANT_READ", Integer::New(isolate, 
    SSL_ERROR_WANT_READ));
  SET_VALUE(isolate, module, "SSL_ERROR_WANT_WRITE", Integer::New(isolate, 
    SSL_ERROR_WANT_WRITE));
  SET_VALUE(isolate, module, "SSL_ERROR_SSL", Integer::New(isolate, 
    SSL_ERROR_SSL));
  SET_VALUE(isolate, module, "SSL_ERROR_WANT_X509_LOOKUP", Integer::New(isolate, 
    SSL_ERROR_WANT_X509_LOOKUP));
  SET_VALUE(isolate, module, "SSL_ERROR_SSL", Integer::New(isolate, 
    SSL_ERROR_SSL));
  SET_VALUE(isolate, module, "SSL_ERROR_SYSCALL", Integer::New(isolate, 
    SSL_ERROR_SYSCALL));
  SET_VALUE(isolate, module, "SSL_ERROR_ZERO_RETURN", Integer::New(isolate, 
    SSL_ERROR_ZERO_RETURN));
  SET_VALUE(isolate, module, "SSL_ERROR_WANT_CONNECT", Integer::New(isolate, 
    SSL_ERROR_WANT_CONNECT));
  SET_VALUE(isolate, module, "SSL_ERROR_WANT_ACCEPT", Integer::New(isolate, 
    SSL_ERROR_WANT_ACCEPT));
  SET_VALUE(isolate, module, "SSL_ERROR_WANT_ASYNC", Integer::New(isolate, 
    SSL_ERROR_WANT_ASYNC));
  SET_VALUE(isolate, module, "SSL_ERROR_WANT_ASYNC_JOB", Integer::New(isolate, 
    SSL_ERROR_WANT_ASYNC_JOB));
  SET_VALUE(isolate, module, "SSL_ERROR_WANT_CLIENT_HELLO_CB", 
    Integer::New(isolate, SSL_ERROR_WANT_CLIENT_HELLO_CB));

  SET_VALUE(isolate, module, "SSL_OP_ALL", BigInt::New(isolate, 
    SSL_OP_ALL));
  SET_VALUE(isolate, module, "SSL_OP_NO_RENEGOTIATION", BigInt::New(isolate, 
    SSL_OP_NO_RENEGOTIATION));
  SET_VALUE(isolate, module, "SSL_OP_NO_COMPRESSION", BigInt::New(isolate, 
    SSL_OP_NO_COMPRESSION));    
  SET_VALUE(isolate, module, "SSL_OP_NO_SSLv3", BigInt::New(isolate, 
    SSL_OP_NO_SSLv3));
  SET_VALUE(isolate, module, "SSL_OP_NO_TLSv1", BigInt::New(isolate, 
    SSL_OP_NO_TLSv1));
  SET_VALUE(isolate, module, "SSL_OP_NO_TLSv1_1", BigInt::New(isolate, 
    SSL_OP_NO_TLSv1_1));
  SET_VALUE(isolate, module, "SSL_OP_NO_DTLSv1", BigInt::New(isolate, 
    SSL_OP_NO_DTLSv1));
  SET_VALUE(isolate, module, "SSL_OP_NO_DTLSv1_2", BigInt::New(isolate, 
    SSL_OP_NO_DTLSv1_2));

  SET_METHOD(isolate, module, "serverContext", ServerContext);
  SET_METHOD(isolate, module, "clientContext", ClientContext);
  SET_METHOD(isolate, module, "destroyContext", DestroyContext);
  SET_METHOD(isolate, module, "acceptSocket", AcceptSocket);
  SET_METHOD(isolate, module, "connectSocket", ConnectSocket);
  SET_METHOD(isolate, module, "getServerName", GetServerName);
  SET_METHOD(isolate, module, "setCiphers", SetCiphers);
  SET_METHOD(isolate, module, "handshake", Handshake);
  SET_METHOD(isolate, module, "error", Error);
  SET_METHOD(isolate, module, "read", Read);
  SET_METHOD(isolate, module, "write", Write);
  SET_METHOD(isolate, module, "shutdown", Shutdown);
  SET_METHOD(isolate, module, "setSNICallback", SetSNICallback);
  SET_METHOD(isolate, module, "setContext", SetContext);
  SET_METHOD(isolate, module, "getCertificate", GetCertificate);

  SET_METHOD(isolate, module, "free", Free);
  SET_MODULE(isolate, target, "tls", module);
}
