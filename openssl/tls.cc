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
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  args.GetReturnValue().Set(Integer::New(isolate, SSL_shutdown(ssl)));
}

void just::tls::Free(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  SSL_free(ssl);
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::tls::Read(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
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
  char* dest = (char*)buf->Data() + off;
  args.GetReturnValue().Set(Integer::New(isolate, SSL_write(ssl, 
    (void*)dest, len)));
}

void just::tls::Handshake(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  SSL* ssl = (SSL*)ab->GetAlignedPointerFromInternalField(1);
  args.GetReturnValue().Set(Integer::New(isolate, SSL_do_handshake(ssl)));
}

void just::tls::AcceptSocket(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Uint32Value(context).ToChecked();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  SSL_CTX* ctx = (SSL_CTX*)ab->GetAlignedPointerFromInternalField(1);
  SSL* ssl = SSL_new(ctx);
  SSL_set_fd(ssl, fd);
  SSL_set_accept_state(ssl);
  Local<ArrayBuffer> buf = args[2].As<ArrayBuffer>();
  buf->SetAlignedPointerInInternalField(1, ssl);
  args.GetReturnValue().Set(Integer::New(isolate, SSL_do_handshake(ssl)));
}

void just::tls::ConnectSocket(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Uint32Value(context).ToChecked();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  SSL_CTX* ctx = (SSL_CTX*)ab->GetAlignedPointerFromInternalField(1);
  SSL* ssl = SSL_new(ctx);
  SSL_set_fd(ssl, fd);
  SSL_set_connect_state(ssl);
  Local<ArrayBuffer> buf = args[2].As<ArrayBuffer>();
  buf->SetAlignedPointerInInternalField(1, ssl);
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
  SET_METHOD(isolate, module, "serverContext", ServerContext);
  SET_METHOD(isolate, module, "clientContext", ClientContext);
  SET_METHOD(isolate, module, "destroyContext", DestroyContext);
  SET_METHOD(isolate, module, "acceptSocket", AcceptSocket);
  SET_METHOD(isolate, module, "connectSocket", ConnectSocket);
  SET_METHOD(isolate, module, "handshake", Handshake);
  SET_METHOD(isolate, module, "error", Error);
  SET_METHOD(isolate, module, "read", Read);
  SET_METHOD(isolate, module, "write", Write);
  SET_METHOD(isolate, module, "shutdown", Shutdown);
  SET_METHOD(isolate, module, "free", Free);
  SET_MODULE(isolate, target, "tls", module);
}
