#include "net.h"

void just::net::Socket(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int domain = args[0]->Int32Value(context).ToChecked();
  int type = args[1]->Int32Value(context).ToChecked();
  int protocol = 0;
  if (args.Length() > 2) {
    protocol = args[2]->Int32Value(context).ToChecked();
  }
  args.GetReturnValue().Set(Integer::New(isolate, socket(domain, type, protocol)));
}

void just::net::SetSockOpt(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int level = args[1]->Int32Value(context).ToChecked();
  int option = args[2]->Int32Value(context).ToChecked();
  int value = args[3]->Int32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, setsockopt(fd, level, 
    option, &value, sizeof(int))));
}

void just::net::GetSockOpt(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int level = args[1]->Int32Value(context).ToChecked();
  int option = args[2]->Int32Value(context).ToChecked();
  int options = 0;
  socklen_t optlen = sizeof(options);
  int r = getsockopt(fd, level, option, &options, &optlen);
  if (r == -1) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  args.GetReturnValue().Set(Integer::New(isolate, options));
}

void just::net::GetSockName(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int domain = args[1]->Int32Value(context).ToChecked();
  if (domain == AF_INET) {
    Local<Array> answer = args[2].As<Array>();
    struct sockaddr_in address;
    socklen_t namelen = (socklen_t)sizeof(address);
    getsockname(fd, (struct sockaddr*)&address, &namelen);
    char addr[INET_ADDRSTRLEN];
    socklen_t size = sizeof(address);
    inet_ntop(AF_INET, &address.sin_addr, addr, size);
    answer->Set(context, 0, String::NewFromUtf8(isolate, addr, 
      v8::NewStringType::kNormal, strlen(addr)).ToLocalChecked()).Check();
    answer->Set(context, 1, Integer::New(isolate, 
      ntohs(address.sin_port))).Check();
    args.GetReturnValue().Set(answer);
  } else {
    struct sockaddr_un address;
    socklen_t namelen = (socklen_t)sizeof(address);
    getsockname(fd, (struct sockaddr*)&address, &namelen);
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
      address.sun_path, v8::NewStringType::kNormal, 
      strlen(address.sun_path)).ToLocalChecked());
  }
}

void just::net::GetPeerName(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int domain = args[1]->Int32Value(context).ToChecked();
  if (domain == AF_INET) {
    Local<Array> answer = args[2].As<Array>();
    struct sockaddr_in address;
    socklen_t namelen = (socklen_t)sizeof(address);
    getpeername(fd, (struct sockaddr*)&address, &namelen);
    char addr[INET_ADDRSTRLEN];
    socklen_t size = sizeof(address);
    inet_ntop(AF_INET, &address.sin_addr, addr, size);
    answer->Set(context, 0, String::NewFromUtf8(isolate, addr, 
      v8::NewStringType::kNormal, strlen(addr)).ToLocalChecked()).Check();
    answer->Set(context, 1, Integer::New(isolate, 
      ntohs(address.sin_port))).Check();
    args.GetReturnValue().Set(answer);
  } else {
    struct sockaddr_un address;
    socklen_t namelen = (socklen_t)sizeof(address);
    getpeername(fd, (struct sockaddr*)&address, &namelen);
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
      address.sun_path, v8::NewStringType::kNormal, 
      strlen(address.sun_path)).ToLocalChecked());
  }
}

void just::net::Listen(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int backlog = args[1]->Int32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, listen(fd, backlog)));
}

void just::net::SocketPair(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int domain = args[0]->Int32Value(context).ToChecked();
  int type = args[1]->Int32Value(context).ToChecked();
  Local<Array> answer = args[2].As<Array>();
  int fd[2];
  int r = socketpair(domain, type, 0, fd);
  if (r == 0) {
    answer->Set(context, 0, Integer::New(isolate, fd[0])).Check();
    answer->Set(context, 1, Integer::New(isolate, fd[1])).Check();
  }
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Pipe(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> answer = args[0].As<Array>();
  int flags = O_CLOEXEC;
  if (args.Length() > 1) {
    flags = args[1]->Int32Value(context).ToChecked();
  }
  int fd[2];
  int r = pipe2(fd, flags);
  if (r == 0) {
    answer->Set(context, 0, Integer::New(isolate, fd[0])).Check();
    answer->Set(context, 1, Integer::New(isolate, fd[1])).Check();
  }
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Connect(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int r = 0;
  if (args.Length() > 2) {
    int socktype = AF_INET;
    String::Utf8Value address(isolate, args[1]);
    int port = args[2]->Int32Value(context).ToChecked();
    struct sockaddr_in server_addr;
    server_addr.sin_family = socktype;
    server_addr.sin_port = htons(port);
    inet_pton(socktype, *address, &(server_addr.sin_addr.s_addr));
    r = connect(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
  } else {
    int socktype = AF_UNIX;
    String::Utf8Value path(isolate, args[1]);
    struct sockaddr_un server_addr;
    server_addr.sun_family = socktype;
    strncpy(server_addr.sun_path, *path, sizeof(server_addr.sun_path));
    r = connect(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
  }
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Bind(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int r = 0;
  if (args.Length() > 2) {
    int socktype = AF_INET;
    String::Utf8Value address(isolate, args[1]);
    int port = args[2]->Int32Value(context).ToChecked();
    struct sockaddr_in server_addr;
    server_addr.sin_family = socktype;
    server_addr.sin_port = htons(port);
    inet_pton(socktype, *address, &(server_addr.sin_addr.s_addr));
    r = bind(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
  } else {
    int socktype = AF_UNIX;
    String::Utf8Value path(isolate, args[1]);
    struct sockaddr_un server_addr;
    server_addr.sun_family = socktype;
    strncpy(server_addr.sun_path, *path, sizeof(server_addr.sun_path));
    r = bind(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
  }
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::GetMacAddress(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();  
  int fd = args[0]->Int32Value(context).ToChecked();
  String::Utf8Value name(isolate, args[1]);
  struct ifreq ifr;
  memset(&ifr,0,sizeof(ifr));
  strncpy(ifr.ifr_name, *name, sizeof(ifr.ifr_name));
  int r = ioctl(fd, SIOCGIFHWADDR, &ifr);
  if (r < 0) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  Local<ArrayBuffer> buf = args[2].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  memcpy(backing->Data(), ifr.ifr_hwaddr.sa_data, 6);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::BindInterface(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  String::Utf8Value name(isolate, args[1]);
  int argc = args.Length();
  int family = AF_PACKET;
  if (argc > 2) {
    family = args[2]->Int32Value(context).ToChecked();
  }
  int protocol = htons(ETH_P_ALL);
  if (argc > 3) {
    protocol = args[3]->Int32Value(context).ToChecked();
  }
  int packetType = 0;
  if (argc > 4) {
    packetType = args[4]->Int32Value(context).ToChecked();
  }
  struct ifreq ifr;
  memset(&ifr,0,sizeof(ifr));
  strncpy(ifr.ifr_name, *name, sizeof(ifr.ifr_name));
  int r = ioctl(fd, SIOCGIFINDEX, &ifr);
  if (r < 0) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  int ifidx = ifr.ifr_ifindex;
  struct sockaddr_ll addr;
  addr.sll_family = family;
  addr.sll_protocol = protocol;
  addr.sll_ifindex = ifidx;
  addr.sll_pkttype = packetType;
  r = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_ll));
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Accept(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, accept(fd, NULL, NULL)));
}

void just::net::Seek(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int argc = args.Length();
  off_t off = 0;
  if (argc > 1) {
    off = args[1]->Int32Value(context).ToChecked();
  }
  int whence = SEEK_SET;
  if (argc > 2) {
    whence = args[2]->Int32Value(context).ToChecked();
  }
  off_t r = lseek(fd, off, whence);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Read(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  Local<ArrayBuffer> buf = args[1].As<ArrayBuffer>();
  int argc = args.Length();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  int off = 0;
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  int len = backing->ByteLength() - off;
  if (argc > 3) {
    len = args[3]->Int32Value(context).ToChecked();
  }
  const char* data = (const char*)backing->Data() + off;
  int r = read(fd, (void*)data, len);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Recv(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  Local<ArrayBuffer> buf = args[1].As<ArrayBuffer>();
  int argc = args.Length();
  int flags = 0;
  int off = 0;
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  int len = backing->ByteLength() - off;
  if (argc > 2) {
    off = args[2]->Int32Value(context).ToChecked();
  }
  if (argc > 3) {
    len = args[3]->Int32Value(context).ToChecked();
  }
  if (argc > 4) {
    flags = args[4]->Int32Value(context).ToChecked();
  }
  const char* data = (const char*)backing->Data() + off;
  int r = recv(fd, (void*)data, len, flags);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Write(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  int argc = args.Length();
  int len = 0;
  if (argc > 2) {
    len = args[2]->Int32Value(context).ToChecked();
  } else {
    len = backing->ByteLength();
  }
  int off = 0;
  if (argc > 3) {
    off = args[3]->Int32Value(context).ToChecked();
  }
  char* buf = (char*)backing->Data() + off;
  args.GetReturnValue().Set(Integer::New(isolate, write(fd, 
    buf, len)));
}

void just::net::WriteString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  String::Utf8Value str(args.GetIsolate(), args[1]);
  int len = str.length();
  args.GetReturnValue().Set(Integer::New(isolate, write(fd, 
    *str, len)));
}

void just::net::Writev(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::net::Send(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> obj;
  int fd = args[0]->Int32Value(context).ToChecked();
  Local<ArrayBuffer> buf = args[1].As<ArrayBuffer>();
  int argc = args.Length();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  int len = backing->ByteLength();
  if (argc > 2) {
    len = args[2]->Int32Value(context).ToChecked();
  }
  int off = 0;
  if (argc > 3) {
    off = args[3]->Int32Value(context).ToChecked();
  }
  int flags = MSG_NOSIGNAL;
  if (argc > 4) {
    flags = args[4]->Int32Value(context).ToChecked();
  }
  char* out = (char*)backing->Data() + off;
  int r = send(fd, out, len, flags);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::SendString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  String::Utf8Value str(args.GetIsolate(), args[1]);
  int argc = args.Length();
  int flags = MSG_NOSIGNAL;
  if (argc > 2) {
    flags = args[2]->Int32Value(context).ToChecked();
  }
  int len = str.length();
  args.GetReturnValue().Set(Integer::New(isolate, send(fd, 
    *str, len, flags)));
}

void just::net::Close(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, close(fd)));
}

void just::net::Shutdown(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int how = SHUT_RDWR;
  if (args.Length() > 1) {
    how = args[1]->Int32Value(context).ToChecked();
  }
  args.GetReturnValue().Set(Integer::New(isolate, shutdown(fd, how)));
}

void just::net::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> net = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, net, "socket", Socket);
  SET_METHOD(isolate, net, "setsockopt", SetSockOpt);
  SET_METHOD(isolate, net, "getsockopt", GetSockOpt);
  SET_METHOD(isolate, net, "listen", Listen);
  SET_METHOD(isolate, net, "connect", Connect);
  SET_METHOD(isolate, net, "socketpair", SocketPair);
  SET_METHOD(isolate, net, "pipe", Pipe);
  SET_METHOD(isolate, net, "bind", Bind);
  SET_METHOD(isolate, net, "bindInterface", BindInterface);
  SET_METHOD(isolate, net, "getMacAddress", GetMacAddress);
  SET_METHOD(isolate, net, "accept", Accept);
  SET_METHOD(isolate, net, "read", Read);
  SET_METHOD(isolate, net, "seek", Seek);
  SET_METHOD(isolate, net, "recv", Recv);
  SET_METHOD(isolate, net, "write", Write);
  SET_METHOD(isolate, net, "writeString", WriteString);
  SET_METHOD(isolate, net, "writev", Writev);
  SET_METHOD(isolate, net, "send", Send);
  SET_METHOD(isolate, net, "sendString", SendString);
  SET_METHOD(isolate, net, "close", Close);
  SET_METHOD(isolate, net, "shutdown", Shutdown);
  SET_METHOD(isolate, net, "getsockname", GetSockName);
  SET_METHOD(isolate, net, "getpeername", GetPeerName);

  SET_VALUE(isolate, net, "AF_INET", Integer::New(isolate, AF_INET));
  SET_VALUE(isolate, net, "AF_UNIX", Integer::New(isolate, AF_UNIX));

  // Address Families - bits/socket.h
  SET_VALUE(isolate, net, "AF_INET6", Integer::New(isolate, AF_INET6));
  SET_VALUE(isolate, net, "AF_NETLINK", Integer::New(isolate, AF_NETLINK));
  SET_VALUE(isolate, net, "AF_PACKET", Integer::New(isolate, AF_PACKET));
  SET_VALUE(isolate, net, "AF_RDS", Integer::New(isolate, AF_RDS));
  SET_VALUE(isolate, net, "AF_PPPOX", Integer::New(isolate, AF_PPPOX));
  SET_VALUE(isolate, net, "AF_BLUETOOTH", Integer::New(isolate, AF_BLUETOOTH));
  SET_VALUE(isolate, net, "AF_ALG", Integer::New(isolate, AF_ALG));
  SET_VALUE(isolate, net, "AF_VSOCK", Integer::New(isolate, AF_VSOCK));
  SET_VALUE(isolate, net, "AF_KCM", Integer::New(isolate, AF_KCM));
  SET_VALUE(isolate, net, "AF_LLC", Integer::New(isolate, AF_LLC));
  SET_VALUE(isolate, net, "AF_IB", Integer::New(isolate, AF_IB));
  SET_VALUE(isolate, net, "AF_MPLS", Integer::New(isolate, AF_MPLS));
  SET_VALUE(isolate, net, "AF_CAN", Integer::New(isolate, AF_CAN));

  // Protocol Families - bits/socket.h
  SET_VALUE(isolate, net, "AF_PACKET", Integer::New(isolate, AF_PACKET));
  SET_VALUE(isolate, net, "ETH_P_ALL", Integer::New(isolate, ETH_P_ALL));

  // packet types - linux/if_packet.h
  SET_VALUE(isolate, net, "PACKET_BROADCAST", Integer::New(isolate, PACKET_BROADCAST));
  SET_VALUE(isolate, net, "PACKET_MULTICAST", Integer::New(isolate, PACKET_MULTICAST));
  SET_VALUE(isolate, net, "PACKET_OTHERHOST", Integer::New(isolate, PACKET_OTHERHOST));
  SET_VALUE(isolate, net, "PACKET_OUTGOING", Integer::New(isolate, PACKET_OUTGOING));
  SET_VALUE(isolate, net, "PACKET_HOST", Integer::New(isolate, PACKET_HOST));
  SET_VALUE(isolate, net, "PACKET_LOOPBACK", Integer::New(isolate, PACKET_LOOPBACK));

  SET_VALUE(isolate, net, "SOCK_STREAM", Integer::New(isolate, SOCK_STREAM));
  SET_VALUE(isolate, net, "SOCK_DGRAM", Integer::New(isolate, SOCK_DGRAM));
  SET_VALUE(isolate, net, "SOCK_RAW", Integer::New(isolate, SOCK_RAW));
  SET_VALUE(isolate, net, "SOCK_SEQPACKET", Integer::New(isolate, SOCK_SEQPACKET));
  SET_VALUE(isolate, net, "SOCK_RDM", Integer::New(isolate, SOCK_RDM));
  SET_VALUE(isolate, net, "PF_PACKET", Integer::New(isolate, PF_PACKET));

  SET_VALUE(isolate, net, "SOCK_NONBLOCK", Integer::New(isolate, 
    SOCK_NONBLOCK));
  SET_VALUE(isolate, net, "SOCK_CLOEXEC", Integer::New(isolate, SOCK_CLOEXEC));
  SET_VALUE(isolate, net, "SOL_SOCKET", Integer::New(isolate, SOL_SOCKET));
  SET_VALUE(isolate, net, "SO_ERROR", Integer::New(isolate, SO_ERROR));
  SET_VALUE(isolate, net, "SO_REUSEADDR", Integer::New(isolate, SO_REUSEADDR));
  SET_VALUE(isolate, net, "SO_REUSEPORT", Integer::New(isolate, SO_REUSEPORT));
  SET_VALUE(isolate, net, "SO_INCOMING_CPU", Integer::New(isolate, 
    SO_INCOMING_CPU));
  SET_VALUE(isolate, net, "IPPROTO_TCP", Integer::New(isolate, IPPROTO_TCP));
  SET_VALUE(isolate, net, "IPPROTO_UDP", Integer::New(isolate, IPPROTO_UDP));
  SET_VALUE(isolate, net, "IPPROTO_ICMP", Integer::New(isolate, IPPROTO_ICMP));

  SET_VALUE(isolate, net, "IPPROTO_IGMP", Integer::New(isolate, IPPROTO_IGMP));
  SET_VALUE(isolate, net, "IPPROTO_IPV6", Integer::New(isolate, IPPROTO_IPV6));
  SET_VALUE(isolate, net, "IPPROTO_AH", Integer::New(isolate, IPPROTO_AH));
  SET_VALUE(isolate, net, "IPPROTO_ENCAP", Integer::New(isolate, IPPROTO_ENCAP));
  SET_VALUE(isolate, net, "IPPROTO_COMP", Integer::New(isolate, IPPROTO_COMP));
  SET_VALUE(isolate, net, "IPPROTO_PIM", Integer::New(isolate, IPPROTO_PIM));
  SET_VALUE(isolate, net, "IPPROTO_SCTP", Integer::New(isolate, IPPROTO_SCTP));
  SET_VALUE(isolate, net, "IPPROTO_UDPLITE", Integer::New(isolate, IPPROTO_UDPLITE));
  SET_VALUE(isolate, net, "IPPROTO_RAW", Integer::New(isolate, IPPROTO_RAW));

  SET_VALUE(isolate, net, "TCP_NODELAY", Integer::New(isolate, TCP_NODELAY));
  SET_VALUE(isolate, net, "SO_KEEPALIVE", Integer::New(isolate, SO_KEEPALIVE));
  SET_VALUE(isolate, net, "SOMAXCONN", Integer::New(isolate, SOMAXCONN));
  SET_VALUE(isolate, net, "O_NONBLOCK", Integer::New(isolate, O_NONBLOCK));
  SET_VALUE(isolate, net, "O_CLOEXEC", Integer::New(isolate, O_CLOEXEC));
  
  SET_VALUE(isolate, net, "EAGAIN", Integer::New(isolate, EAGAIN));
  SET_VALUE(isolate, net, "EWOULDBLOCK", Integer::New(isolate, EWOULDBLOCK));
  SET_VALUE(isolate, net, "EINTR", Integer::New(isolate, EINTR));
  SET_VALUE(isolate, net, "SEEK_SET", Integer::New(isolate, SEEK_SET));
  SET_VALUE(isolate, net, "SEEK_CUR", Integer::New(isolate, SEEK_CUR));
  SET_VALUE(isolate, net, "SEEK_END", Integer::New(isolate, SEEK_END));
  SET_MODULE(isolate, target, "net", net);
}
