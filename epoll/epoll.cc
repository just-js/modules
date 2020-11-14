#include "epoll.h"

void just::epoll::EpollCtl(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int loopfd = args[0]->Int32Value(context).ToChecked();
  int action = args[1]->Int32Value(context).ToChecked();
  int fd = args[2]->Int32Value(context).ToChecked();
  int mask = args[3]->Int32Value(context).ToChecked();
  struct epoll_event event;
  event.events = mask;
  event.data.fd = fd;
  args.GetReturnValue().Set(Integer::New(isolate, epoll_ctl(loopfd, 
    action, fd, &event)));
}

void just::epoll::EpollCreate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int flags = args[0]->Int32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, epoll_create1(flags)));
}

void just::epoll::EpollWait(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int loopfd = args[0]->Int32Value(context).ToChecked();
  Local<ArrayBuffer> buf = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  struct epoll_event* events = (struct epoll_event*)backing->Data();
  int size = backing->ByteLength() / 12;
  int timeout = -1;
  int argc = args.Length();
  if (argc > 2) {
    timeout = args[2]->Int32Value(context).ToChecked();
  }
  if (argc > 3) {
    Local<ArrayBuffer> buf = args[3].As<ArrayBuffer>();
    std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
    sigset_t* set = static_cast<sigset_t*>(backing->Data());
    int r = epoll_pwait(loopfd, events, size, timeout, set);
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  int r = epoll_wait(loopfd, events, size, timeout);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::epoll::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> epoll = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, epoll, "control", EpollCtl);
  SET_METHOD(isolate, epoll, "create", EpollCreate);
  SET_METHOD(isolate, epoll, "wait", EpollWait);
  SET_VALUE(isolate, epoll, "EPOLL_CTL_ADD", Integer::New(isolate, 
    EPOLL_CTL_ADD));
  SET_VALUE(isolate, epoll, "EPOLL_CTL_MOD", Integer::New(isolate, 
    EPOLL_CTL_MOD));
  SET_VALUE(isolate, epoll, "EPOLL_CTL_DEL", Integer::New(isolate, 
    EPOLL_CTL_DEL));
  SET_VALUE(isolate, epoll, "EPOLLET", Integer::New(isolate, (int)EPOLLET)); // TODO: overflow error if i don't cast
  SET_VALUE(isolate, epoll, "EPOLLIN", Integer::New(isolate, EPOLLIN));
  SET_VALUE(isolate, epoll, "EPOLLOUT", Integer::New(isolate, EPOLLOUT));
  SET_VALUE(isolate, epoll, "EPOLLERR", Integer::New(isolate, EPOLLERR));
  SET_VALUE(isolate, epoll, "EPOLLHUP", Integer::New(isolate, EPOLLHUP));
  SET_VALUE(isolate, epoll, "EPOLLEXCLUSIVE", Integer::New(isolate, 
    EPOLLEXCLUSIVE));
  SET_VALUE(isolate, epoll, "EPOLLONESHOT", Integer::New(isolate, 
    EPOLLONESHOT));
  SET_VALUE(isolate, epoll, "EPOLL_CLOEXEC", Integer::New(isolate, 
    EPOLL_CLOEXEC));
  SET_MODULE(isolate, target, "epoll", epoll);
}
