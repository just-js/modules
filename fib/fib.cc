#include "fib.h"

uint32_t fibonacci(uint32_t n) {
  if (n <= 1) return 1;
  return fibonacci(n-1) + fibonacci(n-2);
}

void just::fib::Fibonacci(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();  
  args.GetReturnValue().Set(Integer::New(isolate, fibonacci(args[0]->Int32Value(context).ToChecked())));
}

void just::fib::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "calculate", Fibonacci);
  SET_MODULE(isolate, target, "fib", module);
}
