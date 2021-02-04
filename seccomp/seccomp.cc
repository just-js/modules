#include "seccomp.h"

void just::seccomp::Create(const FunctionCallbackInfo<Value> &args) {
	uint32_t seccomp_default_action = SCMP_ACT_KILL_PROCESS;
	if (args.Length() > 0) {
		seccomp_default_action = Local<Integer>::Cast(args[0])->Value();
	}
	Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> tpl = ObjectTemplate::New(isolate);
  tpl->SetInternalFieldCount(1);
  Local<Object> ctx = tpl->NewInstance(context).ToLocalChecked();
	scmp_filter_ctx seccomp_ctx = seccomp_init(seccomp_default_action);
	if (seccomp_ctx == NULL) {
		return;
	}
  ctx->SetAlignedPointerInInternalField(0, seccomp_ctx);
  args.GetReturnValue().Set(ctx);
}

void just::seccomp::AddRule(const FunctionCallbackInfo<Value> &args) {
  Local<Object> ctx = args[0].As<Object>();
  scmp_filter_ctx seccomp_ctx = (scmp_filter_ctx)ctx->GetAlignedPointerFromInternalField(0);
	Isolate* isolate = args.GetIsolate();
	uint32_t seccomp_default_action = SCMP_ACT_ALLOW;
	int syscall_nr = Local<Integer>::Cast(args[1])->Value();
	if (args.Length() > 2) {
		seccomp_default_action = Local<Integer>::Cast(args[2])->Value();
	}
  args.GetReturnValue().Set(Integer::New(isolate, seccomp_rule_add_exact(seccomp_ctx, seccomp_default_action, syscall_nr, 0)));
}

void just::seccomp::Load(const FunctionCallbackInfo<Value> &args) {
	args.GetReturnValue().Set(Integer::New(args.GetIsolate(), seccomp_load((scmp_filter_ctx)args[0].As<Object>()->GetAlignedPointerFromInternalField(0))));
}

void just::seccomp::Release(const FunctionCallbackInfo<Value> &args) {
	seccomp_release((scmp_filter_ctx)args[0].As<Object>()->GetAlignedPointerFromInternalField(0));
}

void just::seccomp::GetName(const FunctionCallbackInfo<Value> &args) {
  char* name = seccomp_syscall_resolve_num_arch(SCMP_ARCH_X86_64, Local<Integer>::Cast(args[0])->Value());
	if (name == NULL) return;
  args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), name, 
    NewStringType::kNormal, strnlen(name, SYSCALL_NAME_MAX_LEN)).ToLocalChecked());
}

void just::seccomp::GetNumber(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value syscall(isolate, args[0]);
  args.GetReturnValue().Set(Integer::New(isolate, seccomp_syscall_resolve_name(*syscall)));
}

void just::seccomp::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  SET_METHOD(isolate, module, "create", Create);
  SET_METHOD(isolate, module, "addRule", AddRule);
  SET_METHOD(isolate, module, "load", Load);
  SET_METHOD(isolate, module, "release", Release);

  SET_METHOD(isolate, module, "getName", GetName);
  SET_METHOD(isolate, module, "getNumber", GetNumber);
  SET_VALUE(isolate, module, "SYSCALL_NAME_MAX_LEN", Integer::New(isolate, SYSCALL_NAME_MAX_LEN));
  SET_VALUE(isolate, module, "SCMP_ACT_KILL_PROCESS", Integer::New(isolate, SCMP_ACT_KILL_PROCESS));
  SET_VALUE(isolate, module, "SCMP_ACT_KILL_THREAD", Integer::New(isolate, SCMP_ACT_KILL_THREAD));
 	// Throw a SIGSYS signal
  SET_VALUE(isolate, module, "SCMP_ACT_TRAP", Integer::New(isolate, SCMP_ACT_TRAP));
 	// Allow the syscall to be executed after the action has been logged
  SET_VALUE(isolate, module, "SCMP_ACT_LOG", Integer::New(isolate, SCMP_ACT_LOG));
 	// Allow the syscall to be executed
  SET_VALUE(isolate, module, "SCMP_ACT_ALLOW", Integer::New(isolate, SCMP_ACT_ALLOW));
  SET_MODULE(isolate, target, "seccomp", module);
}
