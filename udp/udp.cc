#include "udp.h"

void just::udp::RecvMsg(const FunctionCallbackInfo<Value> &args) {
  // todo - this would be better if we could store the structs in a buffer and 
  // not have to recreate them for every message
  int fd = Local<Integer>::Cast(args[0])->Value();
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
    args.GetReturnValue().Set(Integer::New(args.GetIsolate(), bytes));
    return;
  }
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  inet_ntop(AF_INET, &a4->sin_addr, ip, iplen);
  answer->Set(context, 0, String::NewFromUtf8(isolate, ip, 
    v8::NewStringType::kNormal, strlen(ip)).ToLocalChecked()).Check();
  answer->Set(context, 1, Integer::New(isolate, ntohs(a4->sin_port))).Check();
  args.GetReturnValue().Set(Integer::New(isolate, bytes));
}

void just::udp::SendMsg(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int argc = args.Length();
  int fd = Local<Integer>::Cast(args[0])->Value();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  int port = Local<Integer>::Cast(args[3])->Value();
  size_t len = backing->ByteLength();
  if (argc > 4) len = Local<Integer>::Cast(args[4])->Value();
  struct iovec buf;
  buf.iov_base = backing->Data();
  buf.iov_len = len;
  struct msghdr h;
  memset(&h, 0, sizeof h);
  struct sockaddr_in client_addr;
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(port);
  if (argc > 5) {
    client_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  } else {
    String::Utf8Value address(isolate, args[2]);
    inet_aton(*address, &client_addr.sin_addr);
  }
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
  SET_VALUE(isolate, module, "SO_BROADCAST", Integer::New(isolate, SO_BROADCAST));
  SET_MODULE(isolate, target, "udp", module);
}
