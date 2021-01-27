#include "sys.h"

uint64_t just::sys::hrtime() {
  struct timespec t;
  clock_t clock_id = CLOCK_MONOTONIC;
  if (clock_gettime(clock_id, &t))
    return 0;
  return t.tv_sec * (uint64_t) 1e9 + t.tv_nsec;
}

void just::sys::WaitPID(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<Int32Array>()->Buffer();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  int *fields = static_cast<int *>(backing->Data());
  int pid = -1;
  if (args.Length() > 1) {
    pid = args[1]->IntegerValue(context).ToChecked();
  }
  fields[1] = waitpid(pid, &fields[0], WNOHANG);
  fields[0] = WEXITSTATUS(fields[0]); 
  args.GetReturnValue().Set(args[0]);
}

void just::sys::Exec(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value filePath(isolate, args[0]);
  Local<Array> arguments = args[1].As<Array>();
  int len = arguments->Length();
  char** argv = (char**)calloc(len + 2, sizeof(char*));
  argv[0] = (char*)calloc(1, filePath.length());
  memcpy(argv[0], *filePath, filePath.length());
  Local<Context> context = isolate->GetCurrentContext();
  int written = 0;
  for (int i = 0; i < len; i++) {
    Local<String> val = 
      arguments->Get(context, i).ToLocalChecked().As<v8::String>();
    argv[i + 1] = (char*)calloc(1, val->Length() + 1);
    val->WriteUtf8(isolate, argv[i + 1], val->Length() + 1, &written, 
      v8::String::HINT_MANY_WRITES_EXPECTED);
  }
  argv[len + 1] = NULL;
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), execvp(*filePath, argv)));
}

void just::sys::Spawn(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  String::Utf8Value filePath(isolate, args[0]);
  String::Utf8Value cwd(isolate, args[1]);
  Local<Array> arguments = args[2].As<Array>();
  int fds[6];
  fds[0] = args[3]->IntegerValue(context).ToChecked(); // STDIN READ
  fds[1] = args[4]->IntegerValue(context).ToChecked(); // STDOUT WRITE
  fds[2] = args[5]->IntegerValue(context).ToChecked(); // STDERR WRITE
  int len = arguments->Length();
  char** argv = (char**)calloc(len + 2, sizeof(char*));
  //char* argv[len + 2];
  int written = 0;
  argv[0] = (char*)calloc(1, filePath.length());
  memcpy(argv[0], *filePath, filePath.length());
  for (int i = 0; i < len; i++) {
    Local<String> val = 
      arguments->Get(context, i).ToLocalChecked().As<v8::String>();
    argv[i + 1] = (char*)calloc(1, val->Length() + 1);
    val->WriteUtf8(isolate, argv[i + 1], val->Length() + 1, &written, 
      v8::String::HINT_MANY_WRITES_EXPECTED);
  }
  argv[len + 1] = NULL;
  pid_t pid = fork();
  if (pid == -1) {
    perror("error forking");
    args.GetReturnValue().Set(Integer::New(isolate, pid));
    for (int i = 0; i < len; i++) {
      free(argv[i]);
    }
    free(argv);
    return;
  }
  if (pid == 0) {
    int r = chdir(*cwd);
    if (r < 0) {
      fprintf(stderr, "error changing directory\n");
      exit(127);
    }
    // todo: check return codes
    // copy the pipes from the parent process into stdio fds
    dup2(fds[0], STDIN_FILENO);
    dup2(fds[1], STDOUT_FILENO);
    dup2(fds[2], STDERR_FILENO);
    // todo: use execve and pass environment
    execvp(*filePath, argv);
    perror("error launching child process");
    for (int i = 0; i < len; i++) {
      free(argv[i]);
    }
    free(argv);
    exit(127);
  } else {
    args.GetReturnValue().Set(Integer::New(isolate, pid));
    for (int i = 0; i < len; i++) {
      free(argv[i]);
    }
    free(argv);
  }
}

void just::sys::HRTime(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<BigUint64Array> b64 = args[0].As<BigUint64Array>();
  Local<ArrayBuffer> ab = b64->Buffer();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  uint64_t *fields = static_cast<uint64_t *>(backing->Data());
  fields[0] = just::sys::hrtime();
  args.GetReturnValue().Set(b64);
}

void just::sys::RunMicroTasks(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  isolate->PerformMicrotaskCheckpoint();
  //MicrotasksScope::PerformCheckpoint(isolate);
}

void just::sys::EnqueueMicrotask(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  isolate->EnqueueMicrotask(args[0].As<Function>());
}

void just::sys::Exit(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int status = args[0]->Int32Value(context).ToChecked();
  exit(status);
}

void just::sys::Fork(const FunctionCallbackInfo<Value>& args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), fork()));
}

void just::sys::Kill(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int pid = args[0]->Int32Value(context).ToChecked();
  int signum = args[1]->Int32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, kill(pid, signum)));
}

//TODO: CPU Info:
/*
https://github.com/nodejs/node/blob/4438852aa16689b841e5ffbca4a24fc36a0fe33c/src/node_os.cc#L101
https://github.com/libuv/libuv/blob/c70dd705bc2adc488ddffcdc12f0c610d116e77b/src/unix/linux-core.c#L610
*/
void just::sys::CPUUsage(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  struct tms stat;
  clock_t c = times(&stat);
  args.GetReturnValue().Set(Integer::New(isolate, c));
  if (c == -1) {
    return;
  }
  Local<Uint32Array> b32 = args[0].As<Uint32Array>();
  Local<ArrayBuffer> ab = b32->Buffer();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  uint32_t *fields = static_cast<uint32_t *>(backing->Data());
  fields[0] = stat.tms_utime;
  fields[1] = stat.tms_stime;
  fields[2] = stat.tms_cutime;
  fields[3] = stat.tms_cstime;
}

void just::sys::GetrUsage(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  struct rusage usage;
  // todo: can use RUSAGE_THREAD to get thread (self) usage
  getrusage(RUSAGE_SELF, &usage);
  Local<Float64Array> array = args[0].As<Float64Array>();
  Local<ArrayBuffer> ab = array->Buffer();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  double *fields = static_cast<double *>(backing->Data());
  fields[0] = (JUST_MICROS_PER_SEC * usage.ru_utime.tv_sec) 
    + usage.ru_utime.tv_usec;
  fields[1] = (JUST_MICROS_PER_SEC * usage.ru_stime.tv_sec) 
    + usage.ru_stime.tv_usec;
  fields[2] = usage.ru_maxrss;
  fields[3] = usage.ru_ixrss;
  fields[4] = usage.ru_idrss;
  fields[5] = usage.ru_isrss;
  fields[6] = usage.ru_minflt;
  fields[7] = usage.ru_majflt;
  fields[8] = usage.ru_nswap;
  fields[9] = usage.ru_inblock;
  fields[10] = usage.ru_oublock;
  fields[11] = usage.ru_msgsnd;
  fields[12] = usage.ru_msgrcv;
  fields[13] = usage.ru_nsignals;
  fields[14] = usage.ru_nvcsw;
  fields[15] = usage.ru_nivcsw;
}

void just::sys::PID(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), getpid()));
}

void just::sys::GetSid(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), getsid(Local<Integer>::Cast(args[0])->Value())));
}

void just::sys::SetSid(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), setsid()));
}

void just::sys::Errno(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), errno));
}

void just::sys::StrError(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int err = args[0]->IntegerValue(context).ToChecked();
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    strerror(err)).ToLocalChecked());
}

void just::sys::Sleep(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int seconds = args[0]->IntegerValue(context).ToChecked();
  sleep(seconds);
}

void just::sys::USleep(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int microseconds = args[0]->IntegerValue(context).ToChecked();
  usleep(microseconds);
}

void just::sys::NanoSleep(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int seconds = args[0]->IntegerValue(context).ToChecked();
  int nanoseconds = args[1]->IntegerValue(context).ToChecked();
  struct timespec sleepfor;
  sleepfor.tv_sec = seconds;
  sleepfor.tv_nsec = nanoseconds;
  nanosleep(&sleepfor, NULL);
}

void just::sys::SharedMemoryUsage(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  v8::SharedMemoryStatistics sm_stats;
  v8::V8::GetSharedMemoryStatistics(&sm_stats);
  Local<Object> o = Object::New(isolate);
  o->Set(context, String::NewFromUtf8Literal(isolate, "readOnlySpaceSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, sm_stats.read_only_space_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "readOnlySpaceUsedSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, sm_stats.read_only_space_used_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, 
    "readOnlySpacePhysicalSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, sm_stats.read_only_space_physical_size())).Check();
  args.GetReturnValue().Set(o);
}

void just::sys::HeapObjectStatistics(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  v8::HeapObjectStatistics obj_stats;
  size_t num_types = isolate->NumberOfTrackedHeapObjectTypes();
  Local<Object> res = Object::New(isolate);
  for (size_t i = 0; i < num_types; i++) {
    bool ok = isolate->GetHeapObjectStatisticsAtLastGC(&obj_stats, i);
    if (ok) {
      Local<Object> o = Object::New(isolate);
      o->Set(context, String::NewFromUtf8Literal(isolate, "subType", 
        NewStringType::kNormal), 
        String::NewFromUtf8(isolate, obj_stats.object_sub_type(), 
        NewStringType::kNormal).ToLocalChecked()).Check();
      o->Set(context, String::NewFromUtf8Literal(isolate, "count", 
        NewStringType::kNormal), 
        Integer::New(isolate, obj_stats.object_count())).Check();
      o->Set(context, String::NewFromUtf8Literal(isolate, "size", 
        NewStringType::kNormal), 
        Integer::New(isolate, obj_stats.object_size())).Check();
      res->Set(context, 
        String::NewFromUtf8(isolate, obj_stats.object_type(), 
        NewStringType::kNormal).ToLocalChecked(),
        o
      ).Check();
    }
  }
  args.GetReturnValue().Set(res);
}

void just::sys::HeapCodeStatistics(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  v8::HeapCodeStatistics code_stats;
  isolate->GetHeapCodeAndMetadataStatistics(&code_stats);
  Local<Object> o = Object::New(isolate);
  o->Set(context, String::NewFromUtf8Literal(isolate, "codeAndMetadataSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, code_stats.code_and_metadata_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "bytecodeAndMetadataSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, code_stats.bytecode_and_metadata_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "externalScriptSourceSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, code_stats.external_script_source_size())).Check();
  args.GetReturnValue().Set(o);
}

void just::sys::HeapSpaceUsage(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  HeapSpaceStatistics s;
  size_t number_of_heap_spaces = isolate->NumberOfHeapSpaces();
  Local<Array> spaces = args[0].As<Array>();
  Local<Object> o = Object::New(isolate);
  HeapStatistics v8_heap_stats;
  isolate->GetHeapStatistics(&v8_heap_stats);
  Local<Object> heaps = Object::New(isolate);
  o->Set(context, String::NewFromUtf8Literal(isolate, "totalHeapSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.total_heap_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "totalPhysicalSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.total_physical_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "usedHeapSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.used_heap_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "totalAvailableSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.total_available_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "heapSizeLimit", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.heap_size_limit())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "totalHeapSizeExecutable", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.total_heap_size_executable())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "totalGlobalHandlesSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.total_global_handles_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "usedGlobalHandlesSize", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.used_global_handles_size())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "mallocedMemory", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.malloced_memory())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "externalMemory", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.external_memory())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "peakMallocedMemory", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.peak_malloced_memory())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "nativeContexts", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.number_of_native_contexts())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "detachedContexts", 
    NewStringType::kNormal), 
    Integer::New(isolate, v8_heap_stats.number_of_detached_contexts())).Check();
  o->Set(context, String::NewFromUtf8Literal(isolate, "heapSpaces", 
    NewStringType::kNormal), heaps).Check();
  for (size_t i = 0; i < number_of_heap_spaces; i++) {
    isolate->GetHeapSpaceStatistics(&s, i);
    Local<Float64Array> array = spaces->Get(context, i)
      .ToLocalChecked().As<Float64Array>();
    Local<ArrayBuffer> ab = array->Buffer();
    std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
    double *fields = static_cast<double *>(backing->Data());
    fields[0] = s.physical_space_size();
    fields[1] = s.space_available_size();
    fields[2] = s.space_size();
    fields[3] = s.space_used_size();
    heaps->Set(context, String::NewFromUtf8(isolate, s.space_name(), 
      NewStringType::kNormal).ToLocalChecked(), array).Check();
  }
  args.GetReturnValue().Set(o);
}

void just::sys::Memcpy(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();

  Local<ArrayBuffer> abdest = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> bdest = abdest->GetBackingStore();
  char *dest = static_cast<char *>(bdest->Data());

  Local<ArrayBuffer> absource = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> bsource = absource->GetBackingStore();
  char *source = static_cast<char *>(bsource->Data());
  int slen = bsource->ByteLength();

  int argc = args.Length();
  int off = 0;
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  int len = slen;
  if (argc > 3) {
    len = args[3]->Int32Value(context).ToChecked();
  }
  int off2 = 0;
  if (argc > 4) {
    off2 = args[4]->Int32Value(context).ToChecked();
  }
  if (len == 0) return;
  dest = dest + off;
  source = source + off2;
  memcpy(dest, source, len);
  args.GetReturnValue().Set(Integer::New(isolate, len));
}

void just::sys::Utf8Length(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<String> str = args[0].As<String>();
  args.GetReturnValue().Set(Integer::New(isolate, str->Utf8Length(isolate)));
}

void just::sys::Calloc(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  uint32_t count = args[0]->Uint32Value(context).ToChecked();
  uint32_t size = 0;
  void* chunk;
  if (args[1]->IsString()) {
    Local<String> str = args[1].As<String>();
    size = str->Utf8Length(isolate);
    chunk = calloc(count, size);
    int written;
    char* next = (char*)chunk;
    for (uint32_t i = 0; i < count; i++) {
      str->WriteUtf8(isolate, next, size, &written, 
        String::HINT_MANY_WRITES_EXPECTED | String::NO_NULL_TERMINATION);
      next += written;
    }
  } else {
    size = args[1]->Uint32Value(context).ToChecked();
    chunk = calloc(count, size);
  }
  bool shared = false;
  if (args.Length() > 2) {
    shared = args[2]->BooleanValue(isolate);
  }
  if (shared) {
    std::unique_ptr<BackingStore> backing =
        SharedArrayBuffer::NewBackingStore(chunk, count * size, 
          just::FreeMemory, nullptr);
    Local<SharedArrayBuffer> ab =
        SharedArrayBuffer::New(isolate, std::move(backing));
    args.GetReturnValue().Set(ab);
  } else {
    std::unique_ptr<BackingStore> backing =
        ArrayBuffer::NewBackingStore(chunk, count * size, just::FreeMemory, nullptr);
    Local<ArrayBuffer> ab =
        ArrayBuffer::New(isolate, std::move(backing));
    args.GetReturnValue().Set(ab);
  }
}

void just::sys::ReadString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  char *data = static_cast<char *>(backing->Data());
  int len = backing->ByteLength();
  int argc = args.Length();
  if (argc > 1) {
    len = args[1]->Int32Value(context).ToChecked();
  }
  int off = 0;
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  char* source = data + off;
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, source, 
    NewStringType::kNormal, len).ToLocalChecked());
}

void just::sys::GetAddress(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  char *data = static_cast<char *>(backing->Data());
  args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)data));
}

void just::sys::WriteString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  Local<String> str = args[1].As<String>();
  int off = 0;
  if (args.Length() > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  char *data = static_cast<char *>(backing->Data());
  char* source = data + off;
  int len = str->Utf8Length(isolate);
  int nchars = 0;
  int written = str->WriteUtf8(isolate, source, len, &nchars, v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION);
  args.GetReturnValue().Set(Integer::New(isolate, written));
}

void just::sys::Fcntl(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int flags = args[1]->Int32Value(context).ToChecked();
  if (args.Length() > 2) {
    int val = args[2]->Int32Value(context).ToChecked();
    args.GetReturnValue().Set(Integer::New(isolate, fcntl(fd, flags, val)));
    return;
  }
  args.GetReturnValue().Set(Integer::New(isolate, fcntl(fd, flags)));
}

void just::sys::Cwd(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  char cwd[PATH_MAX];
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, getcwd(cwd, PATH_MAX), 
    NewStringType::kNormal).ToLocalChecked());
}

void just::sys::Env(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int size = 0;
  while (environ[size]) size++;
  Local<Array> envarr = Array::New(isolate);
  for (int i = 0; i < size; ++i) {
    const char *var = environ[i];
    envarr->Set(context, i, String::NewFromUtf8(isolate, var, 
      NewStringType::kNormal, strlen(var)).ToLocalChecked()).Check();
  }
  args.GetReturnValue().Set(envarr);
}

void just::sys::Timer(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int t1 = args[0]->Int32Value(context).ToChecked();
  int t2 = args[1]->Int32Value(context).ToChecked();
  int argc = args.Length();
  clockid_t cid = CLOCK_MONOTONIC;
  if (argc > 2) {
    cid = (clockid_t)args[2]->Int32Value(context).ToChecked();
  }
  int flags = TFD_NONBLOCK | TFD_CLOEXEC;
  if (argc > 3) {
    flags = args[3]->Int32Value(context).ToChecked();
  }
  int fd = timerfd_create(cid, flags);
  if (fd == -1) {
    args.GetReturnValue().Set(Integer::New(isolate, fd));
    return;
  }
  struct itimerspec ts;
  ts.it_interval.tv_sec = t1 / 1000;
	ts.it_interval.tv_nsec = (t1 % 1000) * 1000000;
	ts.it_value.tv_sec = t2 / 1000;
	ts.it_value.tv_nsec = (t2 % 1000) * 1000000;  
  int r = timerfd_settime(fd, 0, &ts, NULL);
  if (r == -1) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  args.GetReturnValue().Set(Integer::New(isolate, fd));
}

void just::sys::AvailablePages(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  long available_pages = sysconf(_SC_AVPHYS_PAGES);
  args.GetReturnValue().Set(Integer::New(isolate, available_pages));
}

void just::sys::ReadMemory(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<BigInt> start64 = Local<BigInt>::Cast(args[0]);
  Local<BigInt> end64 = Local<BigInt>::Cast(args[1]);
  const uint64_t size = end64->Uint64Value() - start64->Uint64Value();
  void* start = reinterpret_cast<void*>(start64->Uint64Value());
  // TODO: is this correct? will it leak?
  // todo: we should pass the buffer in. change all code where we create objects like this
  std::unique_ptr<BackingStore> backing =
      ArrayBuffer::NewBackingStore(start, size, 
        just::UnwrapMemory, nullptr);
  Local<ArrayBuffer> ab =
      ArrayBuffer::New(isolate, std::move(backing));
  args.GetReturnValue().Set(ab);
}
/*
void just::sys::ShmOpen(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  String::Utf8Value name(isolate, args[0]);
  int argc = args.Length();
  int flags = O_RDONLY;
  if (argc > 1) {
    flags = args[1]->Int32Value(context).ToChecked();
  }
  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  if (argc > 2) {
    mode = args[2]->Int32Value(context).ToChecked();
  }
  args.GetReturnValue().Set(Integer::New(isolate, shm_open(*name, flags, mode)));
}

void just::sys::ShmUnlink(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value name(isolate, args[0]);
  args.GetReturnValue().Set(Integer::New(isolate, shm_unlink(*name)));
}
*/
void just::sys::MMap(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int argc = args.Length();
  int fd = args[0]->Int32Value(context).ToChecked();
  int len = args[1]->Int32Value(context).ToChecked();
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_SHARED;
  size_t offset = 0;
  if (argc > 2) {
    prot = args[2]->Int32Value(context).ToChecked();
  }
  if (argc > 3) {
    flags = args[3]->Int32Value(context).ToChecked();
  }
  if (argc > 4) {
    offset = args[4]->Int32Value(context).ToChecked();
  }
  void* data = mmap(0, len, prot, flags, fd, offset);
  if (data == MAP_FAILED) {
    return;
  }
  std::unique_ptr<BackingStore> backing =
      SharedArrayBuffer::NewBackingStore(data, len, 
        just::FreeMappedMemory, nullptr);
  Local<SharedArrayBuffer> ab =
      SharedArrayBuffer::New(isolate, std::move(backing));
  args.GetReturnValue().Set(ab);
}

void just::sys::MUnmap(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<SharedArrayBuffer> ab = args[0].As<SharedArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  int len = args[1]->Int32Value(context).ToChecked();
  int r = munmap(backing->Data(), len);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::sys::Ioctl(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int flags = args[1]->Int32Value(context).ToChecked();
  if (args.Length() > 2) {
    if (args[2]->IsArrayBuffer()) {
      Local<ArrayBuffer> buf = args[2].As<ArrayBuffer>();
      std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
      args.GetReturnValue().Set(Integer::New(isolate, ioctl(fd, flags, backing->Data())));
      return;
    }
    if (args[2]->IsNumber()) {
      args.GetReturnValue().Set(Integer::New(isolate, ioctl(fd, flags, Local<Integer>::Cast(args[2])->Value())));
      return;
    }
  }
  args.GetReturnValue().Set(Integer::New(isolate, ioctl(fd, flags)));
}

void just::sys::Reboot(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  int flags = RB_POWER_OFF;
  if (args.Length() > 0) {
    flags = Local<Integer>::Cast(args[0])->Value();
  }
  args.GetReturnValue().Set(Integer::New(isolate, reboot(flags)));
}

#ifndef STATIC
void just::sys::DLOpen(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int argc = args.Length();
  int mode = RTLD_LAZY;
  void* handle;
  if (argc > 1) {
    mode = args[1]->Int32Value(context).ToChecked();
  }
  if (argc > 0) {
    String::Utf8Value path(isolate, args[0]);
    handle = dlopen(*path, mode);
    //if (handle == NULL) handle = dlopen(NULL, mode);
  } else {
    handle = dlopen(NULL, mode);
  }
  if (handle == NULL) {
    args.GetReturnValue().Set(v8::Null(isolate));
    return;
  }
  args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)handle));
}

void just::sys::DLSym(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
  // todo: this is very dangerous. need to have a think on how best to do this
  void* handle = reinterpret_cast<void*>(address64->Uint64Value());
  String::Utf8Value name(isolate, args[1]);
  void* ptr = dlsym(handle, *name);
  if (ptr == NULL) {
    args.GetReturnValue().Set(v8::Null(isolate));
    return;
  }
  args.GetReturnValue().Set(BigInt::New(isolate, (uint64_t)ptr));
}

void just::sys::DLError(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  char* err = dlerror();
  if (err == NULL) {
    args.GetReturnValue().Set(v8::Null(isolate));
    return;
  }
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, err, 
    NewStringType::kNormal).ToLocalChecked());
}

void just::sys::DLClose(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<BigInt> address64 = Local<BigInt>::Cast(args[0]);
  void* handle = reinterpret_cast<void*>(address64->Uint64Value());
  int r = dlclose(handle);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}
#endif

void just::sys::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> sys = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, sys, "calloc", Calloc);
  SET_METHOD(isolate, sys, "readString", ReadString);
  SET_METHOD(isolate, sys, "writeString", WriteString);
  SET_METHOD(isolate, sys, "getAddress", GetAddress);
  SET_METHOD(isolate, sys, "fcntl", Fcntl);
  SET_METHOD(isolate, sys, "memcpy", Memcpy);
  SET_METHOD(isolate, sys, "sleep", Sleep);
  SET_METHOD(isolate, sys, "utf8Length", Utf8Length);
  SET_METHOD(isolate, sys, "readMemory", ReadMemory);
  SET_METHOD(isolate, sys, "timer", Timer);
  SET_METHOD(isolate, sys, "heapUsage", HeapSpaceUsage);
  SET_METHOD(isolate, sys, "sharedMemoryUsage", SharedMemoryUsage);
  SET_METHOD(isolate, sys, "heapObjectStatistics", HeapObjectStatistics);
  SET_METHOD(isolate, sys, "heapCodeStatistics", HeapCodeStatistics);
  SET_METHOD(isolate, sys, "pid", PID);
  SET_METHOD(isolate, sys, "fork", Fork);
  SET_METHOD(isolate, sys, "exec", Exec);
  SET_METHOD(isolate, sys, "getsid", GetSid);
  SET_METHOD(isolate, sys, "setsid", SetSid);
  SET_METHOD(isolate, sys, "errno", Errno);
  SET_METHOD(isolate, sys, "strerror", StrError);
  SET_METHOD(isolate, sys, "cpuUsage", CPUUsage);
  SET_METHOD(isolate, sys, "getrUsage", GetrUsage);
  SET_METHOD(isolate, sys, "hrtime", HRTime);
  SET_METHOD(isolate, sys, "cwd", Cwd);
  SET_METHOD(isolate, sys, "env", Env);
  SET_METHOD(isolate, sys, "ioctl", Ioctl);
  SET_METHOD(isolate, sys, "spawn", Spawn);
  SET_METHOD(isolate, sys, "waitpid", WaitPID);
  SET_METHOD(isolate, sys, "runMicroTasks", RunMicroTasks);
  SET_METHOD(isolate, sys, "nextTick", EnqueueMicrotask);
  SET_METHOD(isolate, sys, "exit", Exit);
  SET_METHOD(isolate, sys, "kill", Kill);
  SET_METHOD(isolate, sys, "usleep", USleep);
  SET_METHOD(isolate, sys, "pages", AvailablePages);
  SET_METHOD(isolate, sys, "nanosleep", NanoSleep);
  SET_METHOD(isolate, sys, "mmap", MMap);
  SET_METHOD(isolate, sys, "munmap", MUnmap);
  SET_METHOD(isolate, sys, "reboot", Reboot);
#ifndef STATIC
  SET_METHOD(isolate, sys, "dlopen", DLOpen);
  SET_METHOD(isolate, sys, "dlsym", DLSym);
  SET_METHOD(isolate, sys, "dlerror", DLError);
  SET_METHOD(isolate, sys, "dlclose", DLClose);
  SET_VALUE(isolate, sys, "RTLD_LAZY", Integer::New(isolate, RTLD_LAZY));
  SET_VALUE(isolate, sys, "RTLD_NOW", Integer::New(isolate, RTLD_NOW));
#endif

  //SET_METHOD(isolate, sys, "shmOpen", ShmOpen);
  //SET_METHOD(isolate, sys, "shmUnlink", ShmUnlink);  
  SET_VALUE(isolate, sys, "CLOCK_MONOTONIC", Integer::New(isolate, 
    CLOCK_MONOTONIC));
  SET_VALUE(isolate, sys, "TFD_NONBLOCK", Integer::New(isolate, 
    TFD_NONBLOCK));
  SET_VALUE(isolate, sys, "TFD_CLOEXEC", Integer::New(isolate, 
    TFD_CLOEXEC));
  SET_VALUE(isolate, sys, "FD_CLOEXEC", Integer::New(isolate, 
    FD_CLOEXEC));
  SET_VALUE(isolate, sys, "F_GETFL", Integer::New(isolate, F_GETFL));
  SET_VALUE(isolate, sys, "F_SETFL", Integer::New(isolate, F_SETFL));
  SET_VALUE(isolate, sys, "F_GETFD", Integer::New(isolate, F_GETFD));
  SET_VALUE(isolate, sys, "F_SETFD", Integer::New(isolate, F_SETFD));
  SET_VALUE(isolate, sys, "STDIN_FILENO", Integer::New(isolate, 
    STDIN_FILENO));
  SET_VALUE(isolate, sys, "STDOUT_FILENO", Integer::New(isolate, 
    STDOUT_FILENO));
  SET_VALUE(isolate, sys, "STDERR_FILENO", Integer::New(isolate, 
    STDERR_FILENO));    
  SET_VALUE(isolate, sys, "PROT_READ", Integer::New(isolate, PROT_READ));
  SET_VALUE(isolate, sys, "PROT_WRITE", Integer::New(isolate, PROT_WRITE));
  SET_VALUE(isolate, sys, "MAP_SHARED", Integer::New(isolate, MAP_SHARED));
  SET_VALUE(isolate, sys, "MAP_ANONYMOUS", Integer::New(isolate, MAP_ANONYMOUS));

  SET_VALUE(isolate, sys, "RB_AUTOBOOT", Integer::New(isolate, RB_AUTOBOOT));
  SET_VALUE(isolate, sys, "RB_HALT_SYSTEM", Integer::New(isolate, RB_HALT_SYSTEM));
  SET_VALUE(isolate, sys, "RB_POWER_OFF", Integer::New(isolate, RB_POWER_OFF));
  SET_VALUE(isolate, sys, "RB_SW_SUSPEND", Integer::New(isolate, RB_SW_SUSPEND));

  SET_VALUE(isolate, sys, "TIOCNOTTY", Integer::New(isolate, TIOCNOTTY));
  SET_VALUE(isolate, sys, "TIOCSCTTY", Integer::New(isolate, TIOCSCTTY));

// These don't work on alpine. will have to investigate why not
//  SET_VALUE(isolate, sys, "BYTE_ORDER", Integer::New(isolate, __BYTE_ORDER));
//  SET_VALUE(isolate, sys, "LITTLE_ENDIAN", Integer::New(isolate, __LITTLE_ENDIAN));
//  SET_VALUE(isolate, sys, "BIG_ENDIAN", Integer::New(isolate, __BIG_ENDIAN));
  long cpus = sysconf(_SC_NPROCESSORS_ONLN);
  long physical_pages = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGESIZE);
  SET_VALUE(isolate, sys, "cpus", Integer::New(isolate, 
    cpus));
  SET_VALUE(isolate, sys, "physicalPages", Integer::New(isolate, 
    physical_pages));
  SET_VALUE(isolate, sys, "pageSize", Integer::New(isolate, 
    page_size));
  SET_MODULE(isolate, target, "sys", sys);
}
