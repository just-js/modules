#include "rsync.h"

void just::rsync::Signature(const FunctionCallbackInfo<Value> &args) {
  FILE *basis_file, *sig_file;
  rs_result result;
  int basis_fd = Local<Integer>::Cast(args[0])->Value();
  int sig_fd = Local<Integer>::Cast(args[1])->Value();
  rs_magic_number sig_magic = RS_RK_BLAKE2_SIG_MAGIC;
  if (args.Length() > 2) {
    sig_magic = (rs_magic_number)Local<Integer>::Cast(args[2])->Value();
  }
  int block_len = 0x800;
  if (args.Length() > 3) {
    block_len = Local<Integer>::Cast(args[3])->Value();
  }
  int strong_len = 8;
  if (args.Length() > 4) {
    strong_len = Local<Integer>::Cast(args[4])->Value();
  }

  basis_file = fdopen(basis_fd, "rb");
  sig_file = fdopen(sig_fd, "wb");
  fseek(basis_file, 0, SEEK_SET);
  ftruncate(fileno(sig_file), 0);
  fseek(sig_file, 0, SEEK_SET);
  result = rs_sig_file(basis_file, sig_file, block_len, strong_len, sig_magic, NULL);
  fseek(basis_file, 0, SEEK_SET);
  fseek(sig_file, 0, SEEK_SET);
  free(basis_file);
  free(sig_file);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), result));
}

void just::rsync::Signature2(const FunctionCallbackInfo<Value> &args) {
	Isolate* isolate = args.GetIsolate();
  FILE *basis_file, *sig_file;
  rs_result result;
  rs_magic_number sig_magic = RS_RK_BLAKE2_SIG_MAGIC;

  String::Utf8Value basis_name(isolate, args[0]);
  String::Utf8Value sig_name(isolate, args[1]);
  if (args.Length() > 2) {
    sig_magic = (rs_magic_number)Local<Integer>::Cast(args[2])->Value();
  }
  int block_len = 0x800;
  if (args.Length() > 3) {
    block_len = Local<Integer>::Cast(args[3])->Value();
  }
  int strong_len = 8;
  if (args.Length() > 4) {
    strong_len = Local<Integer>::Cast(args[4])->Value();
  }

  basis_file = rs_file_open(*basis_name, "rb", 1);
  sig_file = rs_file_open(*sig_name, "wb", 1);
  result = rs_sig_file(basis_file, sig_file, block_len, strong_len, sig_magic, NULL);
  rs_file_close(sig_file);
  rs_file_close(basis_file);

  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), result));
}

void just::rsync::Delta(const FunctionCallbackInfo<Value> &args) {
  FILE *sig_file, *new_file, *delta_file;
  rs_result result;
  rs_signature_t *sumset;
  int sig_fd = Local<Integer>::Cast(args[0])->Value();
  int new_fd = Local<Integer>::Cast(args[1])->Value();
  int delta_fd = Local<Integer>::Cast(args[2])->Value();
  sig_file = fdopen(sig_fd, "rb");
  new_file = fdopen(new_fd, "rb");
  delta_file = fdopen(delta_fd, "wb");
  ftruncate(fileno(delta_file), 0);
  fseek(delta_file, 0, SEEK_SET);
  fseek(new_file, 0, SEEK_SET);
  fseek(sig_file, 0, SEEK_SET);
  result = rs_loadsig_file(sig_file, &sumset, NULL);
  if (result != RS_DONE) {
    args.GetReturnValue().Set(Integer::New(args.GetIsolate(), result));
    return;
  }
  if ((result = rs_build_hash_table(sumset)) != RS_DONE) {
    args.GetReturnValue().Set(Integer::New(args.GetIsolate(), result));
    return;
  }
  result = rs_delta_file(sumset, new_file, delta_file, NULL);
  fseek(delta_file, 0, SEEK_SET);
  fseek(new_file, 0, SEEK_SET);
  fseek(sig_file, 0, SEEK_SET);
  free(delta_file);
  free(new_file);
  free(sig_file);
  rs_free_sumset(sumset);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), result));
}

void just::rsync::Patch(const FunctionCallbackInfo<Value> &args) {
  FILE *basis_file, *delta_file, *new_file;
  rs_result result;
  int basis_fd = Local<Integer>::Cast(args[0])->Value();
  int delta_fd = Local<Integer>::Cast(args[1])->Value();
  int new_fd = Local<Integer>::Cast(args[2])->Value();
  basis_file = fdopen(basis_fd, "rb");
  new_file = fdopen(new_fd, "wb");
  delta_file = fdopen(delta_fd, "rb");
  fseek(delta_file, 0, SEEK_SET);
  ftruncate(fileno(new_file), 0);
  fseek(new_file, 0, SEEK_SET);
  fseek(basis_file, 0, SEEK_SET);
  result = rs_patch_file(basis_file, delta_file, new_file, NULL);
  fseek(delta_file, 0, SEEK_SET);
  fseek(new_file, 0, SEEK_SET);
  fseek(basis_file, 0, SEEK_SET);
  free(delta_file);
  free(new_file);
  free(basis_file);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), result));
}

void just::rsync::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "signature", Signature);
  SET_METHOD(isolate, module, "signature2", Signature2);
  SET_METHOD(isolate, module, "delta", Delta);
  SET_METHOD(isolate, module, "patch", Patch);

  SET_VALUE(isolate, module, "RS_DELTA_MAGIC", Integer::New(isolate, RS_DELTA_MAGIC));
  SET_VALUE(isolate, module, "RS_MD4_SIG_MAGIC", Integer::New(isolate, RS_MD4_SIG_MAGIC));
  SET_VALUE(isolate, module, "RS_BLAKE2_SIG_MAGIC", Integer::New(isolate, RS_BLAKE2_SIG_MAGIC));
  SET_VALUE(isolate, module, "RS_RK_MD4_SIG_MAGIC", Integer::New(isolate, RS_RK_MD4_SIG_MAGIC));
  SET_VALUE(isolate, module, "RS_RK_BLAKE2_SIG_MAGIC", Integer::New(isolate, RS_RK_BLAKE2_SIG_MAGIC));
/*
    RS_DONE = 0,                *< Completed successfully.
    RS_BLOCKED = 1,             *< Blocked waiting for more data.
    RS_RUNNING = 2,             *< The job is still running, and not yet
                                 * finished or blocked. (This value should
                                 * never be seen by the application.)
    RS_TEST_SKIPPED = 77,       *< Test neither passed or failed.
    RS_IO_ERROR = 100,          *< Error in file or network IO.
    RS_SYNTAX_ERROR = 101,      *< Command line syntax error.
    RS_MEM_ERROR = 102,         *< Out of memory.
    RS_INPUT_ENDED = 103,       *< Unexpected end of input file, perhaps due
                                 * to a truncated file or dropped network
                                 * connection.
    RS_BAD_MAGIC = 104,         *< Bad magic number at start of stream.
                                 * Probably not a librsync file, or possibly
                                 * the wrong kind of file or from an
                                 * incompatible library version.
    RS_UNIMPLEMENTED = 105,     *< Author is lazy.
    RS_CORRUPT = 106,           *< Unbelievable value in stream.
    RS_INTERNAL_ERROR = 107,    *< Probably a library bug.
    RS_PARAM_ERROR = 108        *< Bad value passed in to library, probably
*/

  SET_MODULE(isolate, target, "rsync", module);
}
