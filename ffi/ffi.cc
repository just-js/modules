#include "ffi.h"

static ffi_type* just_ffi_types[16];

void just::ffi::FfiPrepCif(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> cb = args[0].As<ArrayBuffer>();
  int rtype = args[1]->Int32Value(context).ToChecked();
  Local<Array> params = args[2].As<Array>();
  ffi_status status;
  ffi_abi abi = FFI_DEFAULT_ABI;
  // todo: we have to free this
  ffi_cif* cif = (ffi_cif*)calloc(1, sizeof(ffi_cif));
  unsigned int nargs = params->Length();
  // todo: we have to free this
  ffi_type** argtypes = (ffi_type**)calloc(nargs, sizeof(ffi_type));
  //ffi_type* argtypes[nargs];
  for (int i = 0; i < nargs; i++) {
    Local<Value> p = params->Get(context, i).ToLocalChecked();
    int rtype = p->Int32Value(context).ToChecked();
    argtypes[i] = just_ffi_types[rtype];
  }
  status = ffi_prep_cif(cif, abi, nargs, just_ffi_types[rtype], argtypes);
  if (status != FFI_OK) {
    args.GetReturnValue().Set(Integer::New(isolate, status));
    return;
  }
  cb->SetAlignedPointerInInternalField(1, cif);
  args.GetReturnValue().Set(Integer::New(isolate, status));
}

void just::ffi::FfiCall(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ArrayBuffer> cb = args[0].As<ArrayBuffer>();
  Local<BigInt> address64 = Local<BigInt>::Cast(args[1]);
  void* fn = reinterpret_cast<void*>(address64->Uint64Value());
  String::Utf8Value str(isolate, args[2]);
  char* cstr = *str;
  ffi_cif* cif = (ffi_cif*)cb->GetAlignedPointerFromInternalField(1);
  ffi_status status;
  ffi_arg result;
  int argc = args.Length();
  //ffi_type* fnargs[1];
  void* values[1];
  //fnargs[0] = &ffi_type_pointer;
  values[0] = &cstr;
  //status = ffi_prep_cif(cif, FFI_DEFAULT_ABI, 1, &ffi_type_uint32, fnargs);
  status = ffi_prep_cif(cif, FFI_DEFAULT_ABI, 1, &ffi_type_uint32, cif->arg_types);
  if (status != FFI_OK) {
    args.GetReturnValue().Set(Integer::New(isolate, status));
    return;
  }
  ffi_call(cif, FFI_FN(fn), &result, values);
  args.GetReturnValue().Set(Integer::New(isolate, (uint32_t)result));
}

void just::ffi::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "ffiPrepCif", FfiPrepCif);
  SET_METHOD(isolate, module, "ffiCall", FfiCall);

  just_ffi_types[FFI_TYPE_UINT32] = &ffi_type_uint32;
  just_ffi_types[FFI_TYPE_POINTER] = &ffi_type_pointer;

  SET_VALUE(isolate, module, "FFI_UNIX64", Integer::New(isolate, FFI_UNIX64));
  SET_VALUE(isolate, module, "FFI_OK", Integer::New(isolate, FFI_OK));
  SET_VALUE(isolate, module, "FFI_BAD_TYPEDEF", Integer::New(isolate, FFI_BAD_TYPEDEF));
  SET_VALUE(isolate, module, "FFI_BAD_ABI", Integer::New(isolate, FFI_BAD_ABI));

  SET_VALUE(isolate, module, "FFI_TYPE_VOID", Integer::New(isolate, FFI_TYPE_VOID));
  SET_VALUE(isolate, module, "FFI_TYPE_INT", Integer::New(isolate, FFI_TYPE_INT));
  SET_VALUE(isolate, module, "FFI_TYPE_FLOAT", Integer::New(isolate, FFI_TYPE_FLOAT));
  SET_VALUE(isolate, module, "FFI_TYPE_DOUBLE", Integer::New(isolate, FFI_TYPE_DOUBLE));
  SET_VALUE(isolate, module, "FFI_TYPE_LONGDOUBLE", Integer::New(isolate, FFI_TYPE_LONGDOUBLE));
  SET_VALUE(isolate, module, "FFI_TYPE_UINT8", Integer::New(isolate, FFI_TYPE_UINT8));
  SET_VALUE(isolate, module, "FFI_TYPE_SINT8", Integer::New(isolate, FFI_TYPE_SINT8));
  SET_VALUE(isolate, module, "FFI_TYPE_UINT16", Integer::New(isolate, FFI_TYPE_UINT16));
  SET_VALUE(isolate, module, "FFI_TYPE_SINT16", Integer::New(isolate, FFI_TYPE_SINT16));
  SET_VALUE(isolate, module, "FFI_TYPE_UINT32", Integer::New(isolate, FFI_TYPE_UINT32));
  SET_VALUE(isolate, module, "FFI_TYPE_SINT32", Integer::New(isolate, FFI_TYPE_SINT32));
  SET_VALUE(isolate, module, "FFI_TYPE_UINT64", Integer::New(isolate, FFI_TYPE_UINT64));
  SET_VALUE(isolate, module, "FFI_TYPE_SINT64", Integer::New(isolate, FFI_TYPE_SINT64));
  SET_VALUE(isolate, module, "FFI_TYPE_STRUCT", Integer::New(isolate, FFI_TYPE_STRUCT));
  SET_VALUE(isolate, module, "FFI_TYPE_POINTER", Integer::New(isolate, FFI_TYPE_POINTER));
  SET_VALUE(isolate, module, "FFI_TYPE_COMPLEX", Integer::New(isolate, FFI_TYPE_COMPLEX));

  SET_MODULE(isolate, target, "ffi", module);
}
