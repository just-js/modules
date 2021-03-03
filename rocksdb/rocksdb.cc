#include "rocksdb.h"

void just::rocksdb::FreeMemory(void* buf, size_t length, void* data) {
  free(buf);
}

void just::rocksdb::Close(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> obj = args[0].As<Object>();
  rocksdb_t* db = (rocksdb_t*)obj->GetAlignedPointerFromInternalField(0);
  rocksdb_close(db);
}

void just::rocksdb::Get(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> obj = args[0].As<Object>();
  rocksdb_t* db = (rocksdb_t*)obj->GetAlignedPointerFromInternalField(0);
  Local<Object> obj2 = args[1].As<Object>();
  rocksdb_readoptions_t* options = (rocksdb_readoptions_t*)obj2->GetAlignedPointerFromInternalField(0);
  String::Utf8Value key(isolate, args[2]);
  unsigned int size = Local<Integer>::Cast(args[4])->Value();
  size_t len;
  char *err = NULL;
  char *returned_value = rocksdb_get(db, options, *key, key.length(), &len, &err);
  if (len > size) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
    return;
  }
  Local<ArrayBuffer> value = args[3].As<ArrayBuffer>();
  void* data = value->GetAlignedPointerFromInternalField(1);
  if (!data) {
    std::shared_ptr<BackingStore> backing = value->GetBackingStore();
    data = backing->Data();
    value->SetAlignedPointerInInternalField(1, data);
  }
  memcpy(data, returned_value, len);
  args.GetReturnValue().Set(Integer::New(isolate, len));
}

void just::rocksdb::Delete(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> obj = args[0].As<Object>();
  rocksdb_t* db = (rocksdb_t*)obj->GetAlignedPointerFromInternalField(0);
  Local<Object> obj2 = args[1].As<Object>();
  rocksdb_writeoptions_t* options = (rocksdb_writeoptions_t*)obj2->GetAlignedPointerFromInternalField(0);
  String::Utf8Value key(isolate, args[2]);
  char *err = NULL;
  rocksdb_delete(db, options, *key, key.length(), &err);
}

void just::rocksdb::PutString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> obj = args[0].As<Object>();
  rocksdb_t* db = (rocksdb_t*)obj->GetAlignedPointerFromInternalField(0);
  Local<Object> obj2 = args[1].As<Object>();
  rocksdb_writeoptions_t* options = (rocksdb_writeoptions_t*)obj2->GetAlignedPointerFromInternalField(0);
  String::Utf8Value key(isolate, args[2]);
  String::Utf8Value value(isolate, args[3]);
  char *err = NULL;
  rocksdb_put(db, options, *key, key.length(), *value, value.length(), &err);
}

void just::rocksdb::Put(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> obj = args[0].As<Object>();
  rocksdb_t* db = (rocksdb_t*)obj->GetAlignedPointerFromInternalField(0);
  Local<Object> obj2 = args[1].As<Object>();
  rocksdb_writeoptions_t* options = (rocksdb_writeoptions_t*)obj2->GetAlignedPointerFromInternalField(0);
  String::Utf8Value key(isolate, args[2]);
  Local<ArrayBuffer> value = args[3].As<ArrayBuffer>();
  void* data = value->GetAlignedPointerFromInternalField(1);
  if (!data) {
    std::shared_ptr<BackingStore> backing = value->GetBackingStore();
    data = backing->Data();
    value->SetAlignedPointerInInternalField(1, data);
  }
  int len = Local<Integer>::Cast(args[4])->Value();
  char *err = NULL;
  rocksdb_put(db, options, *key, key.length(), (char *)data, len, &err);
  if (err != NULL) {
    args.GetReturnValue().Set(String::NewFromOneByte(isolate, (const uint8_t*)err, 
      NewStringType::kNormal, strnlen(err, 1024)).ToLocalChecked());
  }
}

void just::rocksdb::OpenBackupEngine(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj = args[0].As<Object>();
  rocksdb_options_t* options = (rocksdb_options_t*)obj->GetAlignedPointerFromInternalField(0);
  String::Utf8Value DBBackupPath(isolate, args[1]);
  Local<ObjectTemplate> dbTemplate = ObjectTemplate::New(isolate);
  dbTemplate->SetInternalFieldCount(1);
  Local<Object> database = dbTemplate->NewInstance(context).ToLocalChecked();
  char *err = NULL;
  rocksdb_backup_engine_t* be = rocksdb_backup_engine_open(options, *DBBackupPath, &err);
  database->SetAlignedPointerInInternalField(0, be);
  args.GetReturnValue().Set(database);
}

void just::rocksdb::CreateReadOptions(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  rocksdb_readoptions_t *options = rocksdb_readoptions_create();
  Local<ObjectTemplate> optionsTemplate = ObjectTemplate::New(isolate);
  optionsTemplate->SetInternalFieldCount(1);
  Local<Object> obj = optionsTemplate->NewInstance(context).ToLocalChecked();
  obj->SetAlignedPointerInInternalField(0, options);
  args.GetReturnValue().Set(obj);
}

void just::rocksdb::CreateTableOptions(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  rocksdb_block_based_table_options_t *options = rocksdb_block_based_options_create();
  Local<ObjectTemplate> optionsTemplate = ObjectTemplate::New(isolate);
  optionsTemplate->SetInternalFieldCount(1);
  Local<Object> obj = optionsTemplate->NewInstance(context).ToLocalChecked();
  obj->SetAlignedPointerInInternalField(0, options);
  args.GetReturnValue().Set(obj);
}

void just::rocksdb::CreateWriteOptions(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  rocksdb_writeoptions_t *options = rocksdb_writeoptions_create();
  Local<ObjectTemplate> optionsTemplate = ObjectTemplate::New(isolate);
  optionsTemplate->SetInternalFieldCount(1);
  Local<Object> obj = optionsTemplate->NewInstance(context).ToLocalChecked();
  obj->SetAlignedPointerInInternalField(0, options);
  args.GetReturnValue().Set(obj);
}

void just::rocksdb::DestroyRestoreOptions(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_restore_options_t* options = (rocksdb_restore_options_t*)obj->GetAlignedPointerFromInternalField(0);
  rocksdb_restore_options_destroy(options);
}

void just::rocksdb::DestroyReadOptions(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_readoptions_t* options = (rocksdb_readoptions_t*)obj->GetAlignedPointerFromInternalField(0);
  rocksdb_readoptions_destroy(options);
}

void just::rocksdb::DestroyTableOptions(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_block_based_table_options_t* options = (rocksdb_block_based_table_options_t*)obj->GetAlignedPointerFromInternalField(0);
  rocksdb_block_based_options_destroy(options);
}

void just::rocksdb::DestroyWriteOptions(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_writeoptions_t* options = (rocksdb_writeoptions_t*)obj->GetAlignedPointerFromInternalField(0);
  rocksdb_writeoptions_destroy(options);
}

void just::rocksdb::DestroyOptions(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_options_t* options = (rocksdb_options_t*)obj->GetAlignedPointerFromInternalField(0);
  rocksdb_options_destroy(options);
}

void just::rocksdb::CloseBackupEngine(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_backup_engine_t* be = (rocksdb_backup_engine_t*)obj->GetAlignedPointerFromInternalField(0);
  rocksdb_backup_engine_close(be);
}

void just::rocksdb::RestoreFromBackup(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> obj1 = args[0].As<Object>();
  rocksdb_backup_engine_t* be = (rocksdb_backup_engine_t*)obj1->GetAlignedPointerFromInternalField(0);
  String::Utf8Value DBPath(isolate, args[1]);
  String::Utf8Value LogPath(isolate, args[2]);
  Local<Object> obj2 = args[3].As<Object>();
  rocksdb_restore_options_t* options = (rocksdb_restore_options_t*)obj2->GetAlignedPointerFromInternalField(0);
  char *err = NULL;
  rocksdb_backup_engine_restore_db_from_latest_backup(be, *DBPath, *LogPath, options, &err);
}

void just::rocksdb::CreateRestoreOptions(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  rocksdb_restore_options_t *options = rocksdb_restore_options_create();
  Local<ObjectTemplate> optionsTemplate = ObjectTemplate::New(isolate);
  optionsTemplate->SetInternalFieldCount(1);
  Local<Object> obj = optionsTemplate->NewInstance(context).ToLocalChecked();
  obj->SetAlignedPointerInInternalField(0, options);
  args.GetReturnValue().Set(obj);
}

void just::rocksdb::CreateNewBackup(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj1 = args[0].As<Object>();
  rocksdb_backup_engine_t* be = (rocksdb_backup_engine_t*)obj1->GetAlignedPointerFromInternalField(0);
  Local<Object> obj2 = args[1].As<Object>();
  rocksdb_t* db = (rocksdb_t*)obj2->GetAlignedPointerFromInternalField(0);
  char *err = NULL;
  rocksdb_backup_engine_create_new_backup(be, db, &err);
}

void just::rocksdb::CreateOptions(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  rocksdb_options_t *options = rocksdb_options_create();
  Local<ObjectTemplate> optionsTemplate = ObjectTemplate::New(isolate);
  optionsTemplate->SetInternalFieldCount(1);
  Local<Object> obj = optionsTemplate->NewInstance(context).ToLocalChecked();
  obj->SetAlignedPointerInInternalField(0, options);
  args.GetReturnValue().Set(obj);
}

void just::rocksdb::IncreaseParallelism(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj = args[0].As<Object>();
  rocksdb_options_t* options = (rocksdb_options_t*)obj->GetAlignedPointerFromInternalField(0);
  int cpus = args[1]->Uint32Value(context).ToChecked();
  rocksdb_options_increase_parallelism(options, cpus);
}

void just::rocksdb::CreateIfMissing(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> obj = args[0].As<Object>();
  rocksdb_options_t* options = (rocksdb_options_t*)obj->GetAlignedPointerFromInternalField(0);
  bool create_if_missing = args[1]->BooleanValue(isolate);
  rocksdb_options_set_create_if_missing(options, create_if_missing);
}

void just::rocksdb::OptimizeLevelStyleCompaction(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_options_t* options = (rocksdb_options_t*)obj->GetAlignedPointerFromInternalField(0);
  Local<BigInt> b64 = args[1].As<BigInt>();
  rocksdb_options_optimize_level_style_compaction(options, b64->Uint64Value());
}

void just::rocksdb::AllowMmapReads(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_options_t* options = (rocksdb_options_t*)obj->GetAlignedPointerFromInternalField(0);
  bool on = args[1]->BooleanValue(args.GetIsolate());
  rocksdb_options_set_allow_mmap_reads(options, on);
}

void just::rocksdb::OptimizeUniversalStyleCompaction(const FunctionCallbackInfo<Value> &args) {
  Local<Object> obj = args[0].As<Object>();
  rocksdb_options_t* options = (rocksdb_options_t*)obj->GetAlignedPointerFromInternalField(0);
  Local<BigInt> b64 = args[1].As<BigInt>();
  rocksdb_options_optimize_universal_style_compaction(options, b64->Uint64Value());
}

void just::rocksdb::Open(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj = args[0].As<Object>();
  rocksdb_options_t* options = (rocksdb_options_t*)obj->GetAlignedPointerFromInternalField(0);
  Local<ObjectTemplate> dbTemplate = ObjectTemplate::New(isolate);
  dbTemplate->SetInternalFieldCount(1);
  Local<Object> database = dbTemplate->NewInstance(context).ToLocalChecked();
  String::Utf8Value DBPath(isolate, args[1]);
  char *err = NULL;
  rocksdb_t *db;
  if (args.Length() > 2) {
    int ttl = Local<Integer>::Cast(args[2])->Value();
    db = rocksdb_open_with_ttl(options, *DBPath, ttl, &err);
  } else {
    db = rocksdb_open(options, *DBPath, &err);
  }
  database->SetAlignedPointerInInternalField(0, db);
  args.GetReturnValue().Set(database);
}

void just::rocksdb::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  SET_METHOD(isolate, module, "open", Open);
  SET_METHOD(isolate, module, "close", Close);
  SET_METHOD(isolate, module, "get", Get);
  SET_METHOD(isolate, module, "put", Put);
  SET_METHOD(isolate, module, "putString", PutString);
  SET_METHOD(isolate, module, "remove", Delete);

  SET_METHOD(isolate, module, "createOptions", CreateOptions);
  SET_METHOD(isolate, module, "createReadOptions", CreateReadOptions);
  SET_METHOD(isolate, module, "createWriteOptions", CreateWriteOptions);
  SET_METHOD(isolate, module, "createRestoreOptions", CreateRestoreOptions);
  SET_METHOD(isolate, module, "createTableOptions", CreateTableOptions);

  SET_METHOD(isolate, module, "destroyOptions", DestroyOptions);
  SET_METHOD(isolate, module, "destroyReadOptions", DestroyReadOptions);
  SET_METHOD(isolate, module, "destroyWriteOptions", DestroyWriteOptions);
  SET_METHOD(isolate, module, "destroyRestoreOptions", DestroyRestoreOptions);
  SET_METHOD(isolate, module, "destroyTableOptions", DestroyTableOptions);

  SET_METHOD(isolate, module, "openBackupEngine", OpenBackupEngine);
  SET_METHOD(isolate, module, "createNewBackup", CreateNewBackup);
  SET_METHOD(isolate, module, "restoreFromBackup", RestoreFromBackup);
  SET_METHOD(isolate, module, "closeBackupEngine", CloseBackupEngine);

  SET_METHOD(isolate, module, "increaseParallelism", IncreaseParallelism);
  SET_METHOD(isolate, module, "createIfMissing", CreateIfMissing);
  SET_METHOD(isolate, module, "allowMmapReads", AllowMmapReads);  
  SET_METHOD(isolate, module, "optimizeLevelStyleCompaction", OptimizeLevelStyleCompaction);
  SET_METHOD(isolate, module, "optimizeUniversalStyleCompaction", OptimizeUniversalStyleCompaction);

  SET_MODULE(isolate, target, "rocksdb", module);
}
