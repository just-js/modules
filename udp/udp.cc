#include "udp.h"

void just::udp::RecvMsg(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Uint32Value(context).ToChecked();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  Local<Array> answer = args[2].As<Array>();
  struct iovec buf;
  buf.iov_base = backing->Data();
  buf.iov_len = backing->ByteLength();
  char ip[INET_ADDRSTRLEN];
  int iplen = sizeof ip;
  struct sockaddr_storage peer;
  struct msghdr h;
  memset(&h, 0, sizeof(h));
  memset(&peer, 0, sizeof(peer));
  h.msg_name = &peer;
  h.msg_namelen = sizeof(peer);
  h.msg_iov = &buf;
  h.msg_iovlen = 1;
  const sockaddr_in *a4 = reinterpret_cast<const sockaddr_in *>(&peer);
  int bytes = recvmsg(fd, &h, 0);
  if (bytes <= 0) {
    args.GetReturnValue().Set(Integer::New(isolate, bytes));
    return;
  }
  inet_ntop(AF_INET, &a4->sin_addr, ip, iplen);
  answer->Set(context, 0, String::NewFromUtf8(isolate, ip, 
    v8::NewStringType::kNormal, strlen(ip)).ToLocalChecked()).Check();
  answer->Set(context, 1, Integer::New(isolate, ntohs(a4->sin_port))).Check();
  args.GetReturnValue().Set(Integer::New(isolate, bytes));
}

void just::udp::SendMsg(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int argc = args.Length();
  int fd = args[0]->Uint32Value(context).ToChecked();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  String::Utf8Value address(args.GetIsolate(), args[2]);
  int port = args[3]->Uint32Value(context).ToChecked();
  size_t len = backing->ByteLength();
  if (argc > 4) {
    len = args[4]->Uint32Value(context).ToChecked();
  }
  struct iovec buf;
  buf.iov_base = backing->Data();
  buf.iov_len = len;
  struct msghdr h;
  memset(&h, 0, sizeof h);
  struct sockaddr_in client_addr;
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(port);
  inet_aton(*address, &client_addr.sin_addr);
  bzero(&(client_addr.sin_zero), 8);
  h.msg_name = &client_addr;
  h.msg_namelen = sizeof(struct sockaddr_in);
  h.msg_iov = &buf;
  h.msg_iovlen = 1;
  args.GetReturnValue().Set(Integer::New(isolate, sendmsg(fd, &h, 0)));
}

void just::udp::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "sendmsg", SendMsg);
  SET_METHOD(isolate, module, "recvmsg", RecvMsg);
  SET_MODULE(isolate, target, "udp", module);
}
