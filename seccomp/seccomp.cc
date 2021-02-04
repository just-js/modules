#include "seccomp.h"

void just::seccomp::CreateFilter(const FunctionCallbackInfo<Value> &args) {
	scmp_filter_ctx seccomp_ctx;
	uint32_t seccomp_default_action = SCMP_ACT_KILL_PROCESS;
	uint32_t seccomp_syscall_action = SCMP_ACT_ALLOW;
	bool log_not_kill = false;
	char *cur = NULL;
	char syscall_name[SYSCALL_NAME_MAX_LEN] = {0};
	char *syscall_list = getenv(SECCOMP_DEFAULT_ACTION);
	if (syscall_list) {
		log_not_kill = (0 == strncmp(syscall_list, "log", sizeof("log")));
	}
	syscall_list = getenv(SECCOMP_SYSCALL_ALLOW);
	if (syscall_list) {
		seccomp_default_action = log_not_kill ? SCMP_ACT_LOG : SCMP_ACT_KILL_PROCESS;
		seccomp_syscall_action = SCMP_ACT_ALLOW;
	} else if (syscall_list = getenv(SECCOMP_SYSCALL_DENY)) {
		seccomp_default_action = SCMP_ACT_ALLOW;
		seccomp_syscall_action = log_not_kill ? SCMP_ACT_LOG : SCMP_ACT_KILL_PROCESS;
	} else
		return;
	seccomp_ctx = seccomp_init(seccomp_default_action);
	if (NULL == seccomp_ctx) {
		fputs("failed to init seccomp context\n", stderr);
		exit(1);
	}
	fprintf(stderr, "initializing seccomp with default action (%s)\n", get_seccomp_action_name(seccomp_default_action));
	cur = syscall_list;
	while (cur = strchrnul(syscall_list, (int)':')) {
		if ((cur - syscall_list) > (SYSCALL_NAME_MAX_LEN - 1)) {
			fputs("syscall name is too long\n", stderr);
			seccomp_release(seccomp_ctx);
			exit(1);
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
	/* remove our special environment variables, so the sandboxed code
	 * does not see its seccomp configuration
	 */
	if (unsetenv(SECCOMP_DEFAULT_ACTION)) {
		fputs("failed to unset SECCOMP_DEFAULT_ACTION\n", stderr);
		seccomp_release(seccomp_ctx);
		exit(1);
	}
	if (unsetenv(SECCOMP_SYSCALL_ALLOW)) {
		fputs("failed to unset SECCOMP_SYSCALL_ALLOW\n", stderr);
		seccomp_release(seccomp_ctx);
		exit(1);
	}
	if (unsetenv(SECCOMP_SYSCALL_DENY)) {
		fputs("failed to unset SECCOMP_SYSCALL_DENY\n", stderr);
		seccomp_release(seccomp_ctx);
		exit(1);
	}

	if (seccomp_load(seccomp_ctx)) {
		fputs("failed to load the seccomp filter\n", stderr);
		seccomp_release(seccomp_ctx);
		exit(1);
	}

	seccomp_release(seccomp_ctx);
}

void just::seccomp::GetName(const FunctionCallbackInfo<Value> &args) {
  char* name = seccomp_syscall_resolve_num_arch(SCMP_ARCH_X86_64, Local<Integer>::Cast(args[0])->Value());
  args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), name, 
    NewStringType::kNormal, strnlen(name, 128)).ToLocalChecked());
}

void just::seccomp::GetNumber(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value syscall(isolate, args[0]);
  args.GetReturnValue().Set(Integer::New(isolate, seccomp_syscall_resolve_name(*syscall)));
}

void just::seccomp::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "createFilter", CreateFilter);
  SET_METHOD(isolate, module, "getName", GetName);
  SET_METHOD(isolate, module, "getNumber", GetNumber);
  SET_MODULE(isolate, target, "seccomp", module);
}