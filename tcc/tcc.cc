#include "tcc.h"

int add(int a, int b)
{
    return a + b;
}

const char hello[] = "Hello World!";

void just::tcc::Compile(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  TCCState *s;
  int (*func)(int);
  String::Utf8Value my_program(args.GetIsolate(), args[0]);
  s = tcc_new();
  if (!s) {
    fprintf(stderr, "Could not create tcc state\n");
    return;
  }
  tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
  Local<Array> options = args[1].As<Array>();
  int len = options->Length();
  int written = 0;
  for (int i = 0; i < len; i++) {
    Local<String> val = 
      options->Get(context, i).ToLocalChecked().As<v8::String>();
    char* v = (char*)calloc(1, val->Length());
    val->WriteUtf8(isolate, v, val->Length(), &written, 
      v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION);
    tcc_set_options(s, v);
  }
  Local<Array> includes = args[2].As<Array>();
  len = includes->Length();
  for (int i = 0; i < len; i++) {
    Local<String> val = 
      includes->Get(context, i).ToLocalChecked().As<v8::String>();
    char* v = (char*)calloc(1, val->Length());
    val->WriteUtf8(isolate, v, val->Length(), &written, 
      v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION);
    int r = tcc_add_include_path(s, v);
    if (r != 0) {
      fprintf(stderr, "bad return code\n");
    }
  }
  if (tcc_compile_string(s, *my_program) == -1) {
      fprintf(stderr, "Could not compile\n");
      exit(1);
  }
  args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)s));
}

void just::tcc::GetSymbol(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
  TCCState* s = reinterpret_cast<TCCState*>(address64->Uint64Value());
  String::Utf8Value name(isolate, args[1]);
  void* func = tcc_get_symbol(s, *name);
  if (!func) {
    fprintf(stderr, "Could not get symbol\n");
    exit(1);
  }
  args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)func));
}

void just::tcc::AddSymbol(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
  TCCState* s = reinterpret_cast<TCCState*>(address64->Uint64Value());
  String::Utf8Value name(isolate, args[1]);
  address64 = Local<BigInt>::Cast(args[2]);
  void* func = reinterpret_cast<void*>(address64->Uint64Value());
  int r = tcc_add_symbol(s, *name, func);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::tcc::Destroy(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
  TCCState* s = reinterpret_cast<TCCState*>(address64->Uint64Value());
  tcc_delete(s);
}

void just::tcc::Relocate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
  TCCState* s = reinterpret_cast<TCCState*>(address64->Uint64Value());
  int r = tcc_relocate(s, TCC_RELOCATE_AUTO);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::tcc::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "compile", Compile);
  SET_METHOD(isolate, module, "get", GetSymbol);
  SET_METHOD(isolate, module, "add", AddSymbol);
  SET_METHOD(isolate, module, "destroy", Destroy);
  SET_METHOD(isolate, module, "relocate", Relocate);
  SET_MODULE(isolate, target, "tcc", module);
}
