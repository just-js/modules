#include "iouring.h"

void just::iouring::QueueInit(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  //Local<BigInt> entries = args[0].As<BigInt>();
  int entries = Local<Integer>::Cast(args[0])->Value();
	struct io_uring* ring = (struct io_uring*)calloc(1, sizeof(struct io_uring));
	int r = io_uring_queue_init(entries, ring, 0);
  if (r < 0) {
    return;
  }
  Local<ObjectTemplate> ringTemplate = ObjectTemplate::New(isolate);
  ringTemplate->SetInternalFieldCount(1);
  Local<Object> ringObj = ringTemplate->NewInstance(context).ToLocalChecked();
  ringObj->SetAlignedPointerInInternalField(0, ring);
  args.GetReturnValue().Set(ringObj);
}

void just::iouring::QueueExit(const FunctionCallbackInfo<Value> &args) {
  struct io_uring* ring = (struct io_uring*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
	io_uring_queue_exit(ring);
}

void just::iouring::Submit(const FunctionCallbackInfo<Value> &args) {
  struct io_uring* ring = (struct io_uring*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), io_uring_submit(ring)));
}

void just::iouring::GetSQE(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  struct io_uring* ring = (struct io_uring*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
  Local<ObjectTemplate> sqeTemplate = ObjectTemplate::New(isolate);
  sqeTemplate->SetInternalFieldCount(1);
  Local<Object> sqeObj = sqeTemplate->NewInstance(context).ToLocalChecked();
  io_uring_sqe* sqe = io_uring_get_sqe(ring);
  if (!sqe) return;
  sqeObj->SetAlignedPointerInInternalField(0, sqe);
  args.GetReturnValue().Set(sqeObj);
}

void just::iouring::PrepReadV(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  io_uring_sqe* sqe = (io_uring_sqe*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
  int infd = Local<Integer>::Cast(args[1])->Value();
  Local<Array> buffers = args[2].As<Array>();
  //Local<BigInt> offset = args[3].As<BigInt>();
  int offset = Local<Integer>::Cast(args[3])->Value();
  unsigned int nbufs = buffers->Length();
  struct just::iouring::io_data* iodata = (struct just::iouring::io_data*)calloc(nbufs, sizeof(struct just::iouring::io_data));
  iodata->iov = (struct iovec*)calloc(nbufs, sizeof(struct iovec));
  iodata->read = 1;
  iodata->first_offset = offset;
  for (unsigned int i = 0; i < nbufs; i++) {
    Local<Value> val = buffers->Get(context, i).ToLocalChecked();
    Local<ArrayBuffer> ab = val.As<ArrayBuffer>();
    std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
    unsigned char* data = (unsigned char*)backing->Data();
    unsigned int len = backing->ByteLength();
    iodata->read = nbufs;
    iodata->index = i;
    iodata->offset = iodata->first_offset = offset;
    iodata->iov[i].iov_base = data;
    iodata->iov[i].iov_len = len;
  }
	io_uring_prep_readv(sqe, infd, iodata->iov, nbufs, offset);
	io_uring_sqe_set_data(sqe, iodata);
}

void just::iouring::WaitCQE(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  struct io_uring* ring = (struct io_uring*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
	struct io_uring_cqe *cqe;
  int rc = io_uring_wait_cqe(ring, &cqe);
  Local<Array> result = args[1].As<Array>();
  result->Set(context, 0, Integer::New(isolate, rc)).Check();
  args.GetReturnValue().Set(result);
  if (rc < 0) return;
  Local<ObjectTemplate> cqeTemplate = ObjectTemplate::New(isolate);
  cqeTemplate->SetInternalFieldCount(1);
  Local<Object> cqeObj = cqeTemplate->NewInstance(context).ToLocalChecked();
  cqeObj->SetAlignedPointerInInternalField(0, cqe);
  result->Set(context, 1, cqeObj).Check();
}

void just::iouring::PeekCQE(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  struct io_uring* ring = (struct io_uring*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
	struct io_uring_cqe *cqe;
  int rc = io_uring_peek_cqe(ring, &cqe);
  Local<Array> result = args[1].As<Array>();
  result->Set(context, 0, Integer::New(isolate, rc)).Check();
  args.GetReturnValue().Set(result);
  if (rc < 0) return;
  Local<ObjectTemplate> cqeTemplate = ObjectTemplate::New(isolate);
  cqeTemplate->SetInternalFieldCount(1);
  Local<Object> cqeObj = cqeTemplate->NewInstance(context).ToLocalChecked();
  cqeObj->SetAlignedPointerInInternalField(0, cqe);
  result->Set(context, 1, cqeObj).Check();
}

void just::iouring::GetData(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  struct io_uring_cqe* cqe = (struct io_uring_cqe*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
  struct just::iouring::io_data* iodata = (struct just::iouring::io_data*)io_uring_cqe_get_data(cqe);
  Local<Array> result = args[1].As<Array>();
  result->Set(context, 0, Integer::New(isolate, cqe->res)).Check();
  result->Set(context, 1, Integer::New(isolate, iodata->index)).Check();
  args.GetReturnValue().Set(result);
}

void just::iouring::CQESeen(const FunctionCallbackInfo<Value> &args) {
  struct io_uring* ring = (struct io_uring*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
  struct io_uring_cqe* cqe = (struct io_uring_cqe*)args[1].As<Object>()->GetAlignedPointerFromInternalField(0);
  io_uring_cqe_seen(ring, cqe);
}

void just::iouring::PrepWriteV(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  io_uring_sqe* sqe = (io_uring_sqe*)args[0].As<Object>()->GetAlignedPointerFromInternalField(0);
  int infd = Local<Integer>::Cast(args[1])->Value();
  Local<Array> buffers = args[2].As<Array>();
  //Local<BigInt> offset = args[3].As<BigInt>();
  int offset = Local<Integer>::Cast(args[3])->Value();

  unsigned int nbufs = buffers->Length();
  struct just::iouring::io_data* iodata = (struct just::iouring::io_data*)calloc(nbufs, sizeof(struct just::iouring::io_data));
  iodata->iov = (struct iovec*)calloc(nbufs, sizeof(struct iovec));
  iodata->read = 0;
  iodata->offset = iodata->first_offset = offset;
  for (unsigned int i = 0; i < nbufs; i++) {
    Local<Value> val = buffers->Get(context, i).ToLocalChecked();
    Local<ArrayBuffer> ab = val.As<ArrayBuffer>();
    std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
    unsigned char* data = (unsigned char*)backing->Data();
    unsigned int len = backing->ByteLength();
    iodata->read = nbufs;
    iodata->index = i;
    iodata->offset = iodata->first_offset = offset;
    iodata->iov[i].iov_base = data;
    iodata->iov[i].iov_len = len;
  }
	io_uring_prep_writev(sqe, infd, iodata->iov, nbufs, offset);
	io_uring_sqe_set_data(sqe, iodata);
}

void just::iouring::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "queueInit", QueueInit);
  SET_METHOD(isolate, module, "submit", Submit);
  SET_METHOD(isolate, module, "getSQE", GetSQE);
  SET_METHOD(isolate, module, "waitCQE", WaitCQE);
  SET_METHOD(isolate, module, "peekCQE", PeekCQE);
  SET_METHOD(isolate, module, "prepReadV", PrepReadV);
  SET_METHOD(isolate, module, "prepWriteV", PrepWriteV);
  SET_METHOD(isolate, module, "getData", GetData);
  SET_METHOD(isolate, module, "cqeSeen", CQESeen);
  SET_METHOD(isolate, module, "queueExit", QueueExit);
  SET_MODULE(isolate, target, "iouring", module);
}
