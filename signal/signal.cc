#include "signal.h"

void just::signal::SignalFD(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  sigset_t* set = static_cast<sigset_t*>(backing->Data());
  int flags = SFD_NONBLOCK | SFD_CLOEXEC;
  if (args.Length() > 1) {
    flags = args[1]->Int32Value(context).ToChecked();
  }
  int fd = signalfd(-1, set, flags);
  args.GetReturnValue().Set(Integer::New(isolate, fd));
}

void just::signal::SigEmptySet(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  sigset_t* set = static_cast<sigset_t*>(backing->Data());
  int r = sigemptyset(set);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::signal::SigProcMask(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  sigset_t* set = static_cast<sigset_t*>(backing->Data());
  int action = SIG_SETMASK;
  if (args.Length() > 1) {
    action = args[1]->Int32Value(context).ToChecked();
  }
  int direction = 0;
  if (args.Length() > 2) {
    direction = args[2]->Int32Value(context).ToChecked();
  }
  int r = 0;
  if (direction == 1) {
    r = pthread_sigmask(action, NULL, set);
  } else {
    r = pthread_sigmask(action, set, NULL);
  }
  if (r != 0) {
    args.GetReturnValue().Set(BigInt::New(isolate, r));
    return;
  }
  args.GetReturnValue().Set(BigInt::New(isolate, r));
}

void just::signal::SigAddSet(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> buf = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  sigset_t* set = static_cast<sigset_t*>(backing->Data());
  int signum = args[1]->Int32Value(context).ToChecked();
  args.GetReturnValue().Set(BigInt::New(isolate, sigaddset(set, signum)));
}

// todo: rename namespace to "signal" singular
void just::signal::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "sigprocmask", SigProcMask);
  SET_METHOD(isolate, module, "sigemptyset", SigEmptySet);
  SET_METHOD(isolate, module, "sigaddset", SigAddSet);
  SET_METHOD(isolate, module, "signalfd", SignalFD);
  SET_VALUE(isolate, module, "SFD_NONBLOCK", Integer::New(isolate, 
    SFD_NONBLOCK));
  SET_VALUE(isolate, module, "SFD_CLOEXEC", Integer::New(isolate, SFD_CLOEXEC));
  SET_VALUE(isolate, module, "JUST_SIGSAVE", Integer::New(isolate, 1));
  SET_VALUE(isolate, module, "JUST_SIGLOAD", Integer::New(isolate, 0));
  SET_VALUE(isolate, module, "SIG_BLOCK", Integer::New(isolate, SIG_BLOCK));
  SET_VALUE(isolate, module, "SIG_UNBLOCK", Integer::New(isolate, SIG_UNBLOCK));
  SET_VALUE(isolate, module, "SIG_SETMASK", Integer::New(isolate, 
    SIG_SETMASK));
  SET_VALUE(isolate, module, "SIGINT", Integer::New(isolate, SIGINT));
  SET_VALUE(isolate, module, "SIGILL", Integer::New(isolate, SIGILL));
  SET_VALUE(isolate, module, "SIGABRT", Integer::New(isolate, SIGABRT));
  SET_VALUE(isolate, module, "SIGFPE", Integer::New(isolate, SIGFPE));
  SET_VALUE(isolate, module, "SIGSEGV", Integer::New(isolate, SIGSEGV));
  SET_VALUE(isolate, module, "SIGTERM", Integer::New(isolate, SIGTERM));
  /* Historical signals specified by POSIX. */
  SET_VALUE(isolate, module, "SIGHUP", Integer::New(isolate, SIGHUP));
  SET_VALUE(isolate, module, "SIGQUIT", Integer::New(isolate, SIGQUIT));
  SET_VALUE(isolate, module, "SIGTRAP", Integer::New(isolate, SIGTRAP));
  SET_VALUE(isolate, module, "SIGKILL", Integer::New(isolate, SIGKILL));
  SET_VALUE(isolate, module, "SIGPIPE", Integer::New(isolate, SIGPIPE));
  SET_VALUE(isolate, module, "SIGALRM", Integer::New(isolate, SIGALRM));
  /* New(er) POSIX signals (1003.1-2008, 1003.1-2013).  */
  SET_VALUE(isolate, module, "SIGTTIN", Integer::New(isolate, SIGTTIN));
  SET_VALUE(isolate, module, "SIGTTOU", Integer::New(isolate, SIGTTOU));
  SET_VALUE(isolate, module, "SIGXCPU", Integer::New(isolate, SIGXCPU));
  SET_VALUE(isolate, module, "SIGXFSZ", Integer::New(isolate, SIGXFSZ));
  SET_VALUE(isolate, module, "SIGVTALRM", Integer::New(isolate, SIGVTALRM));
  SET_VALUE(isolate, module, "SIGPROF", Integer::New(isolate, SIGPROF));
  /* Nonstandard signals found in all modern POSIX systems
    (including both BSD and Linux).  */
  SET_VALUE(isolate, module, "SIGWINCH", Integer::New(isolate, SIGWINCH));
  /* Not all systems support real-time signals.  bits/signum.h indicates
    that they are supported by overriding __SIGRTMAX to a value greater
    than __SIGRTMIN.  These constants give the kernel-level hard limits,
    but some real-time signals may be used internally by glibc.  Do not
    use these constants in application code; use SIGRTMIN and SIGRTMAX
    (defined in signal.h) instead.  */
  SET_VALUE(isolate, module, "__SIGRTMIN", Integer::New(isolate, __SIGRTMIN));
  SET_VALUE(isolate, module, "__SIGRTMAX", Integer::New(isolate, __SIGRTMAX));
  /* Biggest signal number + 1 (including real-time signals).  */
  SET_VALUE(isolate, module, "_NSIG", Integer::New(isolate, __SIGRTMAX + 1));
  SET_VALUE(isolate, module, "SIGSTKFLT", Integer::New(isolate, SIGSTKFLT));
  SET_VALUE(isolate, module, "SIGPWR", Integer::New(isolate, SIGPWR));
  // Linux Signals - signum.h
  SET_VALUE(isolate, module, "SIGBUS", Integer::New(isolate, SIGBUS));
  SET_VALUE(isolate, module, "SIGUSR1", Integer::New(isolate, SIGUSR1));
  SET_VALUE(isolate, module, "SIGUSR2", Integer::New(isolate, SIGUSR2));
  SET_VALUE(isolate, module, "SIGCHLD", Integer::New(isolate, SIGCHLD));
  SET_VALUE(isolate, module, "SIGCLD", Integer::New(isolate, SIGCLD));
  SET_VALUE(isolate, module, "SIGCONT", Integer::New(isolate, SIGCONT));
  SET_VALUE(isolate, module, "SIGSTOP", Integer::New(isolate, SIGSTOP));
  SET_VALUE(isolate, module, "SIGTSTP", Integer::New(isolate, SIGTSTP));
  SET_VALUE(isolate, module, "SIGURG", Integer::New(isolate, SIGURG));
  SET_VALUE(isolate, module, "SIGPOLL", Integer::New(isolate, SIGPOLL));
  SET_VALUE(isolate, module, "SIGIO", Integer::New(isolate, SIGIO));
  SET_VALUE(isolate, module, "SIGIOT", Integer::New(isolate, SIGIOT));
  SET_VALUE(isolate, module, "SIGSYS", Integer::New(isolate, SIGSYS));
  SET_VALUE(isolate, module, "__SIGRTMAX", Integer::New(isolate, __SIGRTMAX));
  SET_MODULE(isolate, target, "signal", module);
}
