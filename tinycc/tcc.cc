#include "tcc.h"

int add(int a, int b)
{
    return a + b;
}

const char hello[] = "Hello World!";

void just::tcc::Compile(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  TCCState *s;
  int (*func)(int);
  String::Utf8Value my_program(args.GetIsolate(), args[0]);
  s = tcc_new();
  if (!s) {
      fprintf(stderr, "Could not create tcc state\n");
      exit(1);
  }
  tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
  if (tcc_compile_string(s, *my_program) == -1) {
      fprintf(stderr, "Could not compile\n");
      exit(1);
  }
  tcc_add_symbol(s, "add", (const void*)add);
  tcc_add_symbol(s, "hello", hello);
  if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0) {
    fprintf(stderr, "Could not relocate\n");
    exit(1);
  }
  func = (int (*)(int))tcc_get_symbol(s, "foo");
  if (!func) {
    fprintf(stderr, "Could not get foo\n");
    exit(1);
  }
  func(32);
  tcc_delete(s);
}

void just::tcc::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "compile", Compile);
  SET_MODULE(isolate, target, "tcc", module);
}
