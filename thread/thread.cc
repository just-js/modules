#include "thread.h"

void* just::thread::startThread(void *data) {
  threadContext* ctx = (threadContext*)data;
  just::CreateIsolate(ctx->argc, ctx->argv, ctx->main, 
    ctx->main_len, ctx->source, ctx->source_len, &ctx->buf, ctx->fd);
  free(ctx->source);
  free(ctx);
  return NULL;
}

void just::thread::Spawn(const FunctionCallbackInfo<Value> &args) {
  // TODO: we have to free all the allocated memory when the thread finishes
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int argc = args.Length();
  // get source code to execute in thread
  String::Utf8Value source(isolate, args[0]);
  threadContext* ctx = (threadContext*)calloc(1, sizeof(threadContext));
	ctx->source = (char*)calloc(1, source.length());
  memcpy(ctx->source, *source, source.length());
  ctx->source_len = source.length();

  String::Utf8Value main(isolate, args[1]);
	ctx->main = (char*)calloc(1, main.length());
  memcpy(ctx->main, *main, main.length());
  ctx->main_len = main.length();

  ctx->buf.iov_len = 0;
  ctx->fd = 0;
  // we can pass in a set of args for just.args
  if (argc > 2) {
    Local<Array> arguments = args[2].As<Array>();
    int len = arguments->Length();
    ctx->argc = len;
    ctx->argv = (char**)calloc(len + 1, sizeof(char*));
    int written = 0;
    for (int i = 0; i < len; i++) {
      Local<String> val = 
        arguments->Get(context, i).ToLocalChecked().As<v8::String>();
      ctx->argv[i] = (char*)calloc(1, val->Length());
      val->WriteUtf8(isolate, ctx->argv[i], val->Length(), &written, 
        v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION);
    }
    ctx->argv[len] = NULL;
  } else {
    ctx->argc = 1;
    ctx->argv = new char*[2];
    ctx->argv[1] = NULL;
  }
  // shared array buffer. will be in just.buffer variable inside thread
  if (argc > 3) {
    Local<SharedArrayBuffer> ab = args[3].As<SharedArrayBuffer>();
    std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
    ctx->buf.iov_base = backing->Data();
    ctx->buf.iov_len = backing->ByteLength();
  }
  // socketpair fd for IPC
  if (argc > 4) {
    ctx->fd = args[4]->Int32Value(context).ToChecked();
  }
  // overwrite arg[0] with thread name or passed in name for thread
  if (argc > 5) {
    String::Utf8Value name(isolate, args[5]);
    ctx->argv[0] = (char*)calloc(1, name.length());
    memcpy(ctx->argv[0], *name, name.length());
  } else {
    ctx->argv[0] = (char*)calloc(1, 7);
    strncpy(ctx->argv[0], "thread", 7);
  }
  pthread_t tid;
	int r = pthread_create(&tid, NULL, startThread, ctx);
  if (r != 0) {
    // todo: sensible return codes
    args.GetReturnValue().Set(BigInt::New(isolate, r));
    return;
  }
  args.GetReturnValue().Set(BigInt::New(isolate, tid));
}

void just::thread::Join(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<BigInt> bi = args[0]->ToBigInt(context).ToLocalChecked();
  bool lossless = true;
  pthread_t tid = (pthread_t)bi->Uint64Value(&lossless);
  void* tret;
  int r = pthread_join(tid, &tret);
  if (r != 0) {
    args.GetReturnValue().Set(BigInt::New(isolate, r));
    return;
  }
  args.GetReturnValue().Set(BigInt::New(isolate, (long)tret));
}

void just::thread::TryJoin(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<BigInt> bi = args[0]->ToBigInt(context).ToLocalChecked();
  Local<Array> answer = args[1].As<Array>();
  bool lossless = true;
  pthread_t tid = (pthread_t)bi->Uint64Value(&lossless);
  void* tret;
  int r = pthread_tryjoin_np(tid, &tret);
  answer->Set(context, 0, Integer::New(isolate, (long)tret)).Check();
  args.GetReturnValue().Set(BigInt::New(isolate, r));
}

void just::thread::Self(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  pthread_t tid = pthread_self();
  args.GetReturnValue().Set(BigInt::New(isolate, (long)tid));
}

void just::thread::SetAffinity(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<BigInt> bi = args[0]->ToBigInt(context).ToLocalChecked();
  bool lossless = true;
  pthread_t tid = (pthread_t)bi->Uint64Value(&lossless);
  int cpu = args[1]->Int32Value(context).ToChecked();
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu, &cpuset);
  int r = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::thread::SetName(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<BigInt> bi = args[0]->ToBigInt(context).ToLocalChecked();
  bool lossless = true;
  pthread_t tid = (pthread_t)bi->Uint64Value(&lossless);
  String::Utf8Value name(isolate, args[1]);
  int r = pthread_setname_np(tid, *name);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::thread::GetAffinity(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<BigInt> bi = args[0]->ToBigInt(context).ToLocalChecked();
  bool lossless = true;
  pthread_t tid = (pthread_t)bi->Uint64Value(&lossless);
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  int r = pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
  if (r != 0) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  for (int j = 0; j < CPU_SETSIZE; j++) {
      if (CPU_ISSET(j, &cpuset)) {
        r = j;
        break;
      }
  }
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::thread::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "spawn", Spawn);
  SET_METHOD(isolate, module, "join", Join);
  SET_METHOD(isolate, module, "tryJoin", TryJoin);
  SET_METHOD(isolate, module, "self", Self);
  SET_METHOD(isolate, module, "setAffinity", SetAffinity);
  SET_METHOD(isolate, module, "setName", SetName);
  SET_METHOD(isolate, module, "getAffinity", GetAffinity);
  SET_MODULE(isolate, target, "thread", module);
}

