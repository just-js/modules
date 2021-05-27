#include "profiler.h"

void just::profiler::Snapshot(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  String::Utf8Value path(isolate, args[0]);
  FILE* fp = fopen(*path, "w");
  if (fp == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, errno));
    return;
  }
  const v8::HeapSnapshot *snap = isolate->GetHeapProfiler()->TakeHeapSnapshot();
  //const v8::HeapSnapshot* const snap = TakeHeapSnapshot(isolate);
  FileOutputStream stream(fp);
  snap->Serialize(&stream, v8::HeapSnapshot::kJSON);
  int err = 0;
  if (fclose(fp)) err = errno;
  // Work around a deficiency in the API.  The HeapSnapshot object is const
  // but we cannot call HeapProfiler::DeleteAllHeapSnapshots() because that
  // invalidates _all_ snapshots, including those created by other tools.
  const_cast<v8::HeapSnapshot*>(snap)->Delete();
  args.GetReturnValue().Set(Integer::New(isolate, err));
}

void just::profiler::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "snapshot", Snapshot);
  SET_MODULE(isolate, target, "profiler", module);
}
