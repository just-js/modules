#include "pg.h"

void just::pg::Connect(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  String::Utf8Value str(isolate, args[0]);
  PGconn* pq = PQconnectdb(*str);
  ConnStatusType status = PQstatus(pq);
  if(status != CONNECTION_OK) {
    return;
  }
  int fd = PQsocket(pq);
  Local<ObjectTemplate> dbTemplate = ObjectTemplate::New(isolate);
  dbTemplate->SetInternalFieldCount(2);
  SET_VALUE(isolate, dbTemplate, "fd", Integer::New(isolate, fd));
  Local<Object> database = dbTemplate->NewInstance(context).ToLocalChecked();
  database->SetAlignedPointerInInternalField(0, pq);
  args.GetReturnValue().Set(database);
}

void just::pg::Exec(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  String::Utf8Value str(isolate, args[1]);
  PGresult* result = PQexec(pq, *str);
  database->Set(context, String::NewFromUtf8Literal(isolate, "tuples", NewStringType::kNormal), Integer::New(isolate, PQntuples(result))).Check();
  database->Set(context, String::NewFromUtf8Literal(isolate, "fields", NewStringType::kNormal), Integer::New(isolate, PQnfields(result))).Check();
  database->Set(context, String::NewFromUtf8Literal(isolate, "status", NewStringType::kNormal), Integer::New(isolate, PQresultStatus(result))).Check();
  database->SetAlignedPointerInInternalField(1, result);
}

void just::pg::SendPrepare(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  String::Utf8Value name(isolate, args[1]);
  String::Utf8Value sql(isolate, args[2]);
  int argc = args.Length();
  int nparams = argc - 3;
  Oid* oids = (Oid*)calloc(nparams, sizeof(Oid));
  Oid* oid = oids;
  for (int i = 0; i < nparams; i++) {
    *oid = args[i]->Int32Value(context).ToChecked();
    oid++;
  }
  int r = PQsendPrepare(pq, *name, *sql, nparams, oids);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::pg::SendExecPrepared(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  String::Utf8Value name(isolate, args[1]);
  int argc = args.Length();
  int nparams = argc - 2;
  char* paramValues[nparams];
  int paramLengths[nparams];
  int paramFormats[nparams];
  for (int i = 0; i < nparams; i++) {
    String::Utf8Value param(isolate, args[i + 2]);
    paramValues[i] = (char*)calloc(1, strlen(*param));
    strcpy(paramValues[i], *param);
    paramLengths[i] = param.length();
    paramFormats[i] = 0;
  }
  int r = PQsendQueryPrepared(pq, *name, nparams, paramValues, paramLengths, paramFormats, 0);
  for (int i = 0; i < nparams; i++) {
    free(paramValues[i]);
  }
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::pg::GetResult(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  PGresult* result = PQgetResult(pq);
  if (result == nullptr) {
    args.GetReturnValue().Set(Integer::New(isolate, 0));
    return;
  }
  database->Set(context, String::NewFromUtf8Literal(isolate, "tuples", NewStringType::kNormal), Integer::New(isolate, PQntuples(result))).Check();
  database->Set(context, String::NewFromUtf8Literal(isolate, "fields", NewStringType::kNormal), Integer::New(isolate, PQnfields(result))).Check();
  database->Set(context, String::NewFromUtf8Literal(isolate, "status", NewStringType::kNormal), Integer::New(isolate, PQresultStatus(result))).Check();
  database->SetAlignedPointerInInternalField(1, result);
  args.GetReturnValue().Set(Integer::New(isolate, 1));
}

void just::pg::ConsumeInput(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  int r = PQconsumeInput(pq);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::pg::IsBusy(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  int r = PQisBusy(pq);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::pg::SetNonBlocking(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  int on = args[1]->Int32Value(context).ToChecked();
  int r = PQsetnonblocking(pq, on);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::pg::IsNonBlocking(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  int r = PQisnonblocking(pq);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::pg::Flush(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  int r = PQflush(pq);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::pg::SetSingleRowMode(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  int r = PQsetSingleRowMode(pq);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::pg::Prepare(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  String::Utf8Value name(isolate, args[1]);
  String::Utf8Value sql(isolate, args[2]);
  int argc = args.Length();
  int nparams = argc - 3;
  Oid* oids = (Oid*)calloc(nparams, sizeof(Oid));
  Oid* oid = oids;
  for (int i = 0; i < nparams; i++) {
    *oid = args[i]->Int32Value(context).ToChecked();
    oid++;
  }
  PGresult* result = PQprepare(pq, *name, *sql, nparams, oids);
  database->Set(context, String::NewFromUtf8Literal(isolate, "status", NewStringType::kNormal), Integer::New(isolate, PQresultStatus(result))).Check();
  database->SetAlignedPointerInInternalField(1, result);
}

void just::pg::ExecPrepared(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  String::Utf8Value name(isolate, args[1]);
  int argc = args.Length();
  int nparams = argc - 2;
  char* paramValues[nparams];
  int paramLengths[nparams];
  int paramFormats[nparams];
  for (int i = 0; i < nparams; i++) {
    String::Utf8Value param(isolate, args[i + 2]);
    paramValues[i] = (char*)calloc(1, strlen(*param));
    strcpy(paramValues[i], *param);
    paramLengths[i] = param.length();
    paramFormats[i] = 0;
  }
  PGresult* result = PQexecPrepared(pq, *name, nparams, paramValues, paramLengths, paramFormats, 0);
  for (int i = 0; i < nparams; i++) {
    free(paramValues[i]);
  }
  database->Set(context, String::NewFromUtf8Literal(isolate, "tuples", NewStringType::kNormal), Integer::New(isolate, PQntuples(result))).Check();
  database->Set(context, String::NewFromUtf8Literal(isolate, "fields", NewStringType::kNormal), Integer::New(isolate, PQnfields(result))).Check();
  database->Set(context, String::NewFromUtf8Literal(isolate, "status", NewStringType::kNormal), Integer::New(isolate, PQresultStatus(result))).Check();
  database->SetAlignedPointerInInternalField(1, result);
}

void just::pg::GetValue(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGresult* result = (PGresult*)database->GetAlignedPointerFromInternalField(1);
  int tuple = args[1]->Int32Value(context).ToChecked();
  int field = args[2]->Int32Value(context).ToChecked();
  char* rowValue = PQgetvalue(result, tuple, field);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    rowValue, v8::NewStringType::kNormal, 
    strlen(rowValue)).ToLocalChecked());
}

void just::pg::GetType(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGresult* result = (PGresult*)database->GetAlignedPointerFromInternalField(1);
  int field = args[1]->Int32Value(context).ToChecked();
  Oid type = PQftype(result, field);
  args.GetReturnValue().Set(Integer::New(isolate, type));
}

void just::pg::GetSize(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGresult* result = (PGresult*)database->GetAlignedPointerFromInternalField(1);
  int field = args[1]->Int32Value(context).ToChecked();
  int size = PQfsize(result, field);
  args.GetReturnValue().Set(Integer::New(isolate, size));
}

void just::pg::GetFieldName(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> database = args[0].As<Object>();
  PGresult* result = (PGresult*)database->GetAlignedPointerFromInternalField(1);
  int field = args[1]->Int32Value(context).ToChecked();
  char* rowValue = PQfname(result, field);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    rowValue, v8::NewStringType::kNormal, 
    strlen(rowValue)).ToLocalChecked());
}

void just::pg::CommandStatus(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> dbResult = args[0].As<Object>();
  PGresult* result = (PGresult*)dbResult->GetAlignedPointerFromInternalField(0);
  char* status = PQcmdStatus(result);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    status, v8::NewStringType::kNormal, 
    strlen(status)).ToLocalChecked());
}

void just::pg::ResultStatus(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGresult* result = (PGresult*)database->GetAlignedPointerFromInternalField(1);
  char* status = PQresStatus(PQresultStatus(result));
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    status, v8::NewStringType::kNormal, 
    strlen(status)).ToLocalChecked());
}

void just::pg::ErrorMessage(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  char* status = PQerrorMessage(pq);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    status, v8::NewStringType::kNormal, 
    strlen(status)).ToLocalChecked());
}

void just::pg::Close(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGconn* pq = (PGconn*)database->GetAlignedPointerFromInternalField(0);
  PQfinish(pq);
}

void just::pg::Clear(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> database = args[0].As<Object>();
  PGresult* result = (PGresult*)database->GetAlignedPointerFromInternalField(1);
  PQclear(result);
}

void just::pg::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  // blocking operations
  SET_METHOD(isolate, module, "connect", Connect);
  SET_METHOD(isolate, module, "exec", Exec);
  SET_METHOD(isolate, module, "prepare", Prepare);
  SET_METHOD(isolate, module, "execPrepared", ExecPrepared);

  // non blocking operations
  SET_METHOD(isolate, module, "sendPrepare", SendPrepare);
  SET_METHOD(isolate, module, "sendExecPrepared", SendExecPrepared);
  SET_METHOD(isolate, module, "getResult", GetResult);
  SET_METHOD(isolate, module, "consumeInput", ConsumeInput);
  SET_METHOD(isolate, module, "isBusy", IsBusy);
  SET_METHOD(isolate, module, "setNonBlocking", SetNonBlocking);
  SET_METHOD(isolate, module, "isNonBlocking", IsNonBlocking);
  SET_METHOD(isolate, module, "flush", Flush);
  SET_METHOD(isolate, module, "setSingleRowMode", SetSingleRowMode);

  SET_METHOD(isolate, module, "close", Close);
  SET_METHOD(isolate, module, "clear", Clear);
  SET_METHOD(isolate, module, "getValue", GetValue);
  SET_METHOD(isolate, module, "getType", GetType);
  SET_METHOD(isolate, module, "getSize", GetSize);
  SET_METHOD(isolate, module, "getFieldName", GetFieldName);
  SET_METHOD(isolate, module, "commandStatus", CommandStatus);
  SET_METHOD(isolate, module, "resultStatus", ResultStatus);
  SET_METHOD(isolate, module, "errorMessage", ErrorMessage);

  SET_VALUE(isolate, module, "BOOLOID", Integer::New(isolate, BOOLOID));
  SET_VALUE(isolate, module, "CHAROID", Integer::New(isolate, CHAROID));
  SET_VALUE(isolate, module, "INT8OID", Integer::New(isolate, INT8OID));
  SET_VALUE(isolate, module, "INT2OID", Integer::New(isolate, INT2OID));
  SET_VALUE(isolate, module, "INT4OID", Integer::New(isolate, INT4OID));
  SET_VALUE(isolate, module, "TEXTOID", Integer::New(isolate, TEXTOID));
  SET_VALUE(isolate, module, "OIDOID", Integer::New(isolate, OIDOID));
  SET_VALUE(isolate, module, "JSONOID", Integer::New(isolate, JSONOID));
  SET_VALUE(isolate, module, "XMLOID", Integer::New(isolate, XMLOID));
  SET_VALUE(isolate, module, "FLOAT4OID", Integer::New(isolate, FLOAT4OID));
  SET_VALUE(isolate, module, "FLOAT8OID", Integer::New(isolate, FLOAT8OID));
  SET_VALUE(isolate, module, "BPCHAROID", Integer::New(isolate, BPCHAROID));
  SET_VALUE(isolate, module, "VARCHAROID", Integer::New(isolate, VARCHAROID));
  SET_VALUE(isolate, module, "DATEOID", Integer::New(isolate, DATEOID));
  SET_VALUE(isolate, module, "TIMEOID", Integer::New(isolate, TIMEOID));
  SET_VALUE(isolate, module, "TIMESTAMPOID", Integer::New(isolate, TIMESTAMPOID));
  SET_VALUE(isolate, module, "TIMESTAMPTZOID", Integer::New(isolate, TIMESTAMPTZOID));
  SET_VALUE(isolate, module, "BITOID", Integer::New(isolate, BITOID));
  SET_VALUE(isolate, module, "VARBITOID", Integer::New(isolate, VARBITOID));
  SET_VALUE(isolate, module, "NUMERICOID", Integer::New(isolate, NUMERICOID));

  SET_VALUE(isolate, module, "PGRES_EMPTY_QUERY", Integer::New(isolate, PGRES_EMPTY_QUERY));
  SET_VALUE(isolate, module, "PGRES_COMMAND_OK", Integer::New(isolate, PGRES_COMMAND_OK));
  SET_VALUE(isolate, module, "PGRES_TUPLES_OK", Integer::New(isolate, PGRES_TUPLES_OK));
  SET_VALUE(isolate, module, "PGRES_COPY_OUT", Integer::New(isolate, PGRES_COPY_OUT));
  SET_VALUE(isolate, module, "PGRES_COPY_IN", Integer::New(isolate, PGRES_COPY_IN));
  SET_VALUE(isolate, module, "PGRES_BAD_RESPONSE", Integer::New(isolate, PGRES_BAD_RESPONSE));
  SET_VALUE(isolate, module, "PGRES_NONFATAL_ERROR", Integer::New(isolate, PGRES_NONFATAL_ERROR));
  SET_VALUE(isolate, module, "PGRES_FATAL_ERROR", Integer::New(isolate, PGRES_FATAL_ERROR));
  SET_VALUE(isolate, module, "PGRES_COPY_BOTH", Integer::New(isolate, PGRES_COPY_BOTH));
  SET_VALUE(isolate, module, "PGRES_SINGLE_TUPLE", Integer::New(isolate, PGRES_SINGLE_TUPLE));

  SET_MODULE(isolate, target, "pg", module);
}
