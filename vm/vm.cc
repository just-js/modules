#include "vm.h"

void just::vm::CreateContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
  Local<ObjectTemplate> just = ObjectTemplate::New(isolate);
  just::Init(isolate, just);
  global->Set(String::NewFromUtf8Literal(isolate, "just", 
    NewStringType::kNormal), just);
  Local<Context> context = Context::New(isolate, NULL, global);
  context->AllowCodeGenerationFromStrings(false);
  isolate->SetPromiseRejectCallback(PromiseRejectCallback);
  Local<Object> globalInstance = context->Global();
  globalInstance->Set(context, String::NewFromUtf8Literal(isolate, 
    "global", 
    NewStringType::kNormal), globalInstance).Check();
  Local<Value> obj = globalInstance->Get(context, 
    String::NewFromUtf8Literal(
      isolate, "just", 
      NewStringType::kNormal)).ToLocalChecked();
  Local<Object> justInstance = Local<Object>::Cast(obj);
  v8::Persistent<Context, v8::CopyablePersistentTraits<Context>> pContext(isolate, context);
  v8_context* handle = (v8_context*)calloc(1, sizeof(v8_context));
  handle->context = pContext;
  ab->SetAlignedPointerInInternalField(1, handle);
  args.GetReturnValue().Set(justInstance);
}

void just::vm::EnterContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  v8_context* handle = (v8_context*)ab->GetAlignedPointerFromInternalField(1);
  Local<Context> context = handle->context.Get(isolate);
  context->Enter();
}

void just::vm::ExitContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  v8_context* handle = (v8_context*)ab->GetAlignedPointerFromInternalField(1);
  Local<Context> context = handle->context.Get(isolate);
  context->Exit();
}

void just::vm::CompileInContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  v8_context* handle = (v8_context*)ab->GetAlignedPointerFromInternalField(1);
  Local<Context> context = handle->context.Get(isolate);
  TryCatch try_catch(isolate);
  Local<String> source = args[1].As<String>();
  Local<String> path = args[2].As<String>();
  ScriptOrigin baseorigin(path, // resource name
    Integer::New(isolate, 0), // line offset
    Integer::New(isolate, 0),  // column offset
    False(isolate), // is shared cross-origin
    Local<Integer>(),  // script id
    Local<Value>(), // source map url
    False(isolate), // is opaque
    False(isolate), // is wasm
    False(isolate)); // is module
  Local<Script> script;
  ScriptCompiler::Source basescript(source, baseorigin);
  bool ok = ScriptCompiler::Compile(context, &basescript).ToLocal(&script);
  if (!ok) {
    if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
      try_catch.ReThrow();
    }
    return;
  }
  v8::Persistent<Script, v8::CopyablePersistentTraits<Script>> pScript(isolate, script);
  handle->script = pScript;
}

void just::vm::CompileAndRunInContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  v8_context* handle = (v8_context*)ab->GetAlignedPointerFromInternalField(1);
  Local<Context> context = handle->context.Get(isolate);
  TryCatch try_catch(isolate);
  Local<String> source = args[1].As<String>();
  Local<String> path = args[2].As<String>();
  ScriptOrigin baseorigin(path, // resource name
    Integer::New(isolate, 0), // line offset
    Integer::New(isolate, 0),  // column offset
    False(isolate), // is shared cross-origin
    Local<Integer>(),  // script id
    Local<Value>(), // source map url
    False(isolate), // is opaque
    False(isolate), // is wasm
    False(isolate)); // is module
  Local<Script> script;
  ScriptCompiler::Source basescript(source, baseorigin);
  bool ok = ScriptCompiler::Compile(context, &basescript).ToLocal(&script);
  if (!ok) {
    if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
      try_catch.ReThrow();
    }
    return;
  }
  MaybeLocal<Value> result = script->Run(context);
  if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
    try_catch.ReThrow();
    return;
  }
  args.GetReturnValue().Set(result.ToLocalChecked());
}

void just::vm::RunInContext(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  v8_context* handle = (v8_context*)ab->GetAlignedPointerFromInternalField(1);
  Local<Context> context = handle->context.Get(isolate);
  Local<Script> script = handle->script.Get(isolate);
  TryCatch try_catch(isolate);
  MaybeLocal<Value> result = script->Run(context);
  if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
    try_catch.ReThrow();
    return;
  }
  args.GetReturnValue().Set(result.ToLocalChecked());
}

void just::vm::CompileScript(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  TryCatch try_catch(isolate);
  Local<String> source = args[0].As<String>();
  Local<String> path = args[1].As<String>();
  Local<Array> params_buf = args[2].As<Array>();
  Local<Array> context_extensions_buf;
  context_extensions_buf = args[3].As<Array>();
  std::vector<Local<String>> params;
  if (!params_buf.IsEmpty()) {
    for (uint32_t n = 0; n < params_buf->Length(); n++) {
      Local<Value> val;
      if (!params_buf->Get(context, n).ToLocal(&val)) return;
      params.push_back(val.As<String>());
    }
  }
  std::vector<Local<Object>> context_extensions;
  if (!context_extensions_buf.IsEmpty()) {
    for (uint32_t n = 0; n < context_extensions_buf->Length(); n++) {
      Local<Value> val;
      if (!context_extensions_buf->Get(context, n).ToLocal(&val)) return;
      context_extensions.push_back(val.As<Object>());
    }
  }
  ScriptOrigin baseorigin(path, // resource name
    Integer::New(isolate, 0), // line offset
    Integer::New(isolate, 0),  // column offset
    True(isolate));
  Context::Scope scope(context);
  ScriptCompiler::Source basescript(source, baseorigin);
  MaybeLocal<Function> maybe_fn = ScriptCompiler::CompileFunctionInContext(
    context, &basescript, params.size(), params.data(), 0, nullptr, 
    ScriptCompiler::kEagerCompile);
  if (maybe_fn.IsEmpty()) {
    if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
      try_catch.ReThrow();
    }
    return;
  }
  Local<Function> fn = maybe_fn.ToLocalChecked();
  args.GetReturnValue().Set(fn);
}

void just::vm::RunModule(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  TryCatch try_catch(isolate);
  Local<String> source = args[0].As<String>();
  Local<String> path = args[1].As<String>();
  ScriptOrigin baseorigin(path, // resource name
    Integer::New(isolate, 0), // line offset
    Integer::New(isolate, 0),  // column offset
    False(isolate), // is shared cross-origin
    Local<Integer>(),  // script id
    Local<Value>(), // source map url
    False(isolate), // is opaque
    False(isolate), // is wasm
    True(isolate)); // is module
  ScriptCompiler::Source basescript(source, baseorigin);
  Local<Module> module;
  bool ok = ScriptCompiler::CompileModule(isolate, 
    &basescript).ToLocal(&module);
  if (!ok) {
    if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
      try_catch.ReThrow();
    }
    return;
  }
  Maybe<bool> ok2 = module->InstantiateModule(context, OnModuleInstantiate);
  if (ok2.IsNothing()) {
    if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
      try_catch.ReThrow();
    }
    return;
  }
  MaybeLocal<Value> result = module->Evaluate(context);
  if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
    try_catch.ReThrow();
    return;
  }
  args.GetReturnValue().Set(result.ToLocalChecked());
}

void just::vm::RunScript(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetEnteredOrMicrotaskContext();
  TryCatch try_catch(isolate);
  Local<String> source = args[0].As<String>();
  Local<String> path = args[1].As<String>();
  ScriptOrigin baseorigin(path, // resource name
    Integer::New(isolate, 0), // line offset
    Integer::New(isolate, 0),  // column offset
    False(isolate), // is shared cross-origin
    Local<Integer>(),  // script id
    Local<Value>(), // source map url
    False(isolate), // is opaque
    False(isolate), // is wasm
    False(isolate)); // is module
  Local<Script> script;
  ScriptCompiler::Source basescript(source, baseorigin);
  bool ok = ScriptCompiler::Compile(context, &basescript).ToLocal(&script);
  if (!ok) {
    if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
      try_catch.ReThrow();
    }
    return;
  }
  MaybeLocal<Value> result = script->Run(context);
  if (try_catch.HasCaught() && !try_catch.HasTerminated()) {
    try_catch.ReThrow();
    return;
  }
  args.GetReturnValue().Set(result.ToLocalChecked());
}

void just::vm::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> vm = ObjectTemplate::New(isolate);
  // TODO: compile and compileInContext should switch names
  SET_METHOD(isolate, vm, "compile", just::vm::CompileScript);
  SET_METHOD(isolate, vm, "runModule", just::vm::RunModule);
  SET_METHOD(isolate, vm, "runScript", just::vm::RunScript);
  SET_METHOD(isolate, vm, "runInContext", just::vm::RunInContext);
  SET_METHOD(isolate, vm, "compileInContext", just::vm::CompileInContext);
  SET_METHOD(isolate, vm, "compileAndRunInContext", just::vm::CompileAndRunInContext);
  SET_METHOD(isolate, vm, "createContext", just::vm::CreateContext);
  SET_METHOD(isolate, vm, "enterContext", just::vm::EnterContext);
  SET_METHOD(isolate, vm, "exitContext", just::vm::ExitContext);
  SET_MODULE(isolate, target, "vm", vm);
}
