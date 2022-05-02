#include "duckdb.h"

void just::duckdb::CreateConfig(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> configTemplate = ObjectTemplate::New(isolate);
  configTemplate->SetInternalFieldCount(1);
  Local<Object> configObj = configTemplate->NewInstance(context).ToLocalChecked();
  duckdb_config* config = (duckdb_config*)calloc(1, sizeof(duckdb_config));
  if (duckdb_create_config(config) == DuckDBError) {
    return;
  }
  configObj->SetAlignedPointerInInternalField(0, config);
  args.GetReturnValue().Set(configObj);
}

void just::duckdb::Open(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> dbTemplate = ObjectTemplate::New(isolate);
  dbTemplate->SetInternalFieldCount(1);
  Local<Object> dbObj = dbTemplate->NewInstance(context).ToLocalChecked();
  String::Utf8Value filename(isolate, args[0]);
  Local<Object> configObj = args[1].As<Object>();
  duckdb_config* config = (duckdb_config*)configObj->GetAlignedPointerFromInternalField(0);
  duckdb_database db;
  if (duckdb_open_ext(*filename, &db, *config, NULL) == DuckDBError) {
    return;
  }
  dbObj->SetAlignedPointerInInternalField(0, db);
  args.GetReturnValue().Set(dbObj);
}

void just::duckdb::SetConfig(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> configObj = args[0].As<Object>();
  duckdb_config* config = (duckdb_config*)configObj->GetAlignedPointerFromInternalField(0);
  String::Utf8Value key(isolate, args[1]);
  String::Utf8Value value(isolate, args[2]);
  duckdb_state rc = duckdb_set_config(*config, *key, *value);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}

void just::duckdb::Connect(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> dbObj = args[0].As<Object>();
  duckdb_database* db = (duckdb_database*)dbObj->GetAlignedPointerFromInternalField(0);
  duckdb_connection con;
  if (duckdb_connect(db, &con) == DuckDBError) {
    return;
  }
  Local<ObjectTemplate> conTemplate = ObjectTemplate::New(isolate);
  conTemplate->SetInternalFieldCount(1);
  Local<Object> conObj = conTemplate->NewInstance(context).ToLocalChecked();
  conObj->SetAlignedPointerInInternalField(0, con);
  args.GetReturnValue().Set(conObj);
}

void just::duckdb::Query(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> conObj = args[0].As<Object>();
  duckdb_connection* con = (duckdb_connection*)conObj->GetAlignedPointerFromInternalField(0);
  String::Utf8Value sql(isolate, args[1]);
  duckdb_result* result = (duckdb_result*)calloc(1, sizeof(duckdb_result));
  duckdb_state state = duckdb_query(con, *sql, result);
  if (state == DuckDBError) {
    return;
  }
  Local<ObjectTemplate> resultTemplate = ObjectTemplate::New(isolate);
  resultTemplate->SetInternalFieldCount(1);
  Local<Object> resultObj = resultTemplate->NewInstance(context).ToLocalChecked();
  resultObj->SetAlignedPointerInInternalField(0, result);
  args.GetReturnValue().Set(resultObj);
}

void just::duckdb::RowCount(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> resultObj = args[0].As<Object>();
  duckdb_result* result = (duckdb_result*)resultObj->GetAlignedPointerFromInternalField(0);
  args.GetReturnValue().Set(Integer::New(isolate, duckdb_row_count(result)));
}

void just::duckdb::ColumnCount(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> resultObj = args[0].As<Object>();
  duckdb_result* result = (duckdb_result*)resultObj->GetAlignedPointerFromInternalField(0);
  args.GetReturnValue().Set(Integer::New(isolate, duckdb_column_count(result)));
}

void just::duckdb::ValueVarchar(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> resultObj = args[0].As<Object>();
  int row = Local<Integer>::Cast(args[1])->Value();
  int col = Local<Integer>::Cast(args[2])->Value();
  duckdb_result* result = (duckdb_result*)resultObj->GetAlignedPointerFromInternalField(0);
  char* str = duckdb_value_varchar(result, col, row);
  if (str == NULL) return;
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, str, 
    NewStringType::kNormal, strlen(str)).ToLocalChecked());
  duckdb_free(str);
}

void just::duckdb::Free(const FunctionCallbackInfo<Value> &args) {

}

void just::duckdb::DestroyResult(const FunctionCallbackInfo<Value> &args) {

}

void just::duckdb::DestroyConfig(const FunctionCallbackInfo<Value> &args) {

}

void just::duckdb::Disconnect(const FunctionCallbackInfo<Value> &args) {

}

void just::duckdb::Close(const FunctionCallbackInfo<Value> &args) {

}

void just::duckdb::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "createConfig", CreateConfig);
  SET_METHOD(isolate, module, "setConfig", SetConfig);
  SET_METHOD(isolate, module, "open", Open);
  SET_METHOD(isolate, module, "connect", Connect);
  SET_METHOD(isolate, module, "query", Query);

  SET_METHOD(isolate, module, "rowCount", RowCount);
  SET_METHOD(isolate, module, "columnCount", ColumnCount);
  SET_METHOD(isolate, module, "valueVarchar", ValueVarchar);
  SET_METHOD(isolate, module, "free", Free);
  SET_METHOD(isolate, module, "destroyResult", DestroyResult);
  SET_METHOD(isolate, module, "destroyConfig", DestroyConfig);
  SET_METHOD(isolate, module, "disconnect", Disconnect);
  SET_METHOD(isolate, module, "close", Close);

  SET_MODULE(isolate, target, "duckdb", module);
}
