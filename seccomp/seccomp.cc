#include "seccomp.h"

void just::seccomp::Deny(const FunctionCallbackInfo<Value> &args) {
	scmp_filter_ctx seccomp_ctx;
	uint32_t seccomp_default_action = SCMP_ACT_ALLOW;
	uint32_t seccomp_syscall_action = SCMP_ACT_KILL_PROCESS;
	bool log_not_kill = false;
	Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value syscalls(isolate, args[0]);
	if (args.Length() > 1) {
		log_not_kill = args[1]->BooleanValue(isolate);
	}
	if (log_not_kill) {
		seccomp_syscall_action = SCMP_ACT_LOG;
	}
	seccomp_ctx = seccomp_init(seccomp_default_action);
	if (NULL == seccomp_ctx) {
		fputs("failed to init seccomp context\n", stderr);
		args.GetReturnValue().Set(Integer::New(isolate, -1));
		return;
	}
	fprintf(stderr, "initializing seccomp with default action (%s)\n", get_seccomp_action_name(seccomp_default_action));
	char* syscall_list = *syscalls;
	char* cur = syscall_list;
	char syscall_name[SYSCALL_NAME_MAX_LEN] = {0};
	while (cur = strchrnul(syscall_list, (int)':')) {
		if ((cur - syscall_list) > (SYSCALL_NAME_MAX_LEN - 1)) {
			fputs("syscall name is too long\n", stderr);
			args.GetReturnValue().Set(Integer::New(isolate, -1));
			seccomp_release(seccomp_ctx);
			return;
		}
		memcpy(syscall_name, syscall_list, (cur - syscall_list));
		syscall_name[(cur - syscall_list)] = '\0';
		if (0 == strlen(syscall_name)) {
			if ('\0' == *cur)
				break;
			syscall_list = cur + 1;
			continue;
		}
		fprintf(stderr, "adding %s to the process seccomp filter (%s)\n", syscall_name, get_seccomp_action_name(seccomp_syscall_action));
		add_syscall(seccomp_ctx, syscall_name, seccomp_syscall_action);
		if ('\0' == *cur)
			break;
		else
			syscall_list = cur + 1;
	}
	if (seccomp_load(seccomp_ctx)) {
		fputs("failed to load the seccomp filter\n", stderr);
		seccomp_release(seccomp_ctx);
		args.GetReturnValue().Set(Integer::New(isolate, -1));
		return;
	}
	seccomp_release(seccomp_ctx);
	args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::seccomp::Allow(const FunctionCallbackInfo<Value> &args) {
	scmp_filter_ctx seccomp_ctx;
	uint32_t seccomp_default_action = SCMP_ACT_KILL_PROCESS;
	uint32_t seccomp_syscall_action = SCMP_ACT_ALLOW;
	bool log_not_kill = false;
	Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value syscalls(isolate, args[0]);
	if (args.Length() > 1) {
		log_not_kill = args[1]->BooleanValue(isolate);
	}
	if (log_not_kill) {
		seccomp_default_action = SCMP_ACT_LOG;
	}
	seccomp_ctx = seccomp_init(seccomp_default_action);
	if (NULL == seccomp_ctx) {
		fputs("failed to init seccomp context\n", stderr);
		args.GetReturnValue().Set(Integer::New(isolate, -1));
		return;
	}
	fprintf(stderr, "initializing seccomp with default action (%s)\n", get_seccomp_action_name(seccomp_default_action));
	char* syscall_list = *syscalls;
	char* cur = syscall_list;
	char syscall_name[SYSCALL_NAME_MAX_LEN] = {0};
	while (cur = strchrnul(syscall_list, (int)':')) {
		if ((cur - syscall_list) > (SYSCALL_NAME_MAX_LEN - 1)) {
			fputs("syscall name is too long\n", stderr);
			args.GetReturnValue().Set(Integer::New(isolate, -1));
			seccomp_release(seccomp_ctx);
			return;
		}
		memcpy(syscall_name, syscall_list, (cur - syscall_list));
		syscall_name[(cur - syscall_list)] = '\0';
		if (0 == strlen(syscall_name)) {
			if ('\0' == *cur)
				break;
			syscall_list = cur + 1;
			continue;
		}
		fprintf(stderr, "adding %s to the process seccomp filter (%s)\n", syscall_name, get_seccomp_action_name(seccomp_syscall_action));
		add_syscall(seccomp_ctx, syscall_name, seccomp_syscall_action);
		if ('\0' == *cur)
			break;
		else
			syscall_list = cur + 1;
	}
	if (seccomp_load(seccomp_ctx)) {
		fputs("failed to load the seccomp filter\n", stderr);
		seccomp_release(seccomp_ctx);
		args.GetReturnValue().Set(Integer::New(isolate, -1));
		return;
	}
	seccomp_release(seccomp_ctx);
	args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::seccomp::GetName(const FunctionCallbackInfo<Value> &args) {
  char* name = seccomp_syscall_resolve_num_arch(SCMP_ARCH_X86_64, Local<Integer>::Cast(args[0])->Value());
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
  SET_METHOD(isolate, module, "allow", Allow);
  SET_METHOD(isolate, module, "deny", Deny);
  SET_METHOD(isolate, module, "getName", GetName);
  SET_METHOD(isolate, module, "getNumber", GetNumber);
  SET_VALUE(isolate, module, "SYSCALL_NAME_MAX_LEN", Integer::New(isolate, SYSCALL_NAME_MAX_LEN));
  SET_VALUE(isolate, module, "SCMP_ACT_KILL_PROCESS", Integer::New(isolate, SCMP_ACT_KILL_PROCESS));
  SET_VALUE(isolate, module, "SCMP_ACT_KILL_THREAD", Integer::New(isolate, SCMP_ACT_KILL_THREAD));
 	// Throw a SIGSYS signal
  SET_VALUE(isolate, module, "SCMP_ACT_TRAP", Integer::New(isolate, SCMP_ACT_TRAP));
 	// Return the specified error code
  //SET_VALUE(isolate, module, "SCMP_ACT_ERRNO", Integer::New(isolate, SCMP_ACT_ERRNO));
 	// Notify a tracing process with the specified value
  //SET_VALUE(isolate, module, "SCMP_ACT_TRACE", Integer::New(isolate, SCMP_ACT_TRACE));
 	// Allow the syscall to be executed after the action has been logged
  SET_VALUE(isolate, module, "SCMP_ACT_LOG", Integer::New(isolate, SCMP_ACT_LOG));
 	// Allow the syscall to be executed
  SET_VALUE(isolate, module, "SCMP_ACT_ALLOW", Integer::New(isolate, SCMP_ACT_ALLOW));
  SET_MODULE(isolate, target, "seccomp", module);
}
