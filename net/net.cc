#include "net.h"

void just::net::Socket(const FunctionCallbackInfo<Value> &args) {
  int domain = Local<Integer>::Cast(args[0])->Value();
  int type = Local<Integer>::Cast(args[1])->Value();
  int protocol = 0;
  if (args.Length() > 2) {
    protocol = Local<Integer>::Cast(args[2])->Value();
  }
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    socket(domain, type, protocol)));
}

void just::net::SetSockOpt(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  int level = Local<Integer>::Cast(args[1])->Value();
  int option = Local<Integer>::Cast(args[2])->Value();
  if (args[3]->IsNumber()) {
    int value = Local<Integer>::Cast(args[3])->Value();
    args.GetReturnValue().Set(Integer::New(args.GetIsolate(), setsockopt(fd, level, 
      option, &value, sizeof(int))));
    return;
  }
  Local<ArrayBuffer> buf = args[3].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  int size = 0;
  if (args.Length() > 4) {
    size = Local<Integer>::Cast(args[4])->Value();
  } else {
    size = backing->ByteLength();
  }
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    setsockopt(fd, level, option, backing->Data(), size)));
  return;
}

void just::net::GetSockOpt(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  int level = Local<Integer>::Cast(args[1])->Value();
  int option = Local<Integer>::Cast(args[2])->Value();
  int options = 0;
  socklen_t optlen = sizeof(options);
  int r = getsockopt(fd, level, option, &options, &optlen);
  if (r == -1) {
    args.GetReturnValue().Set(Integer::New(args.GetIsolate(), r));
    return;
  }
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), options));
}

void just::net::GetSockName(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int fd = Local<Integer>::Cast(args[0])->Value();
  int domain = Local<Integer>::Cast(args[1])->Value();
  if (domain == AF_INET) {
    Local<Array> answer = args[2].As<Array>();
    struct sockaddr_in address;
    socklen_t namelen = (socklen_t)sizeof(address);
    getsockname(fd, (struct sockaddr*)&address, &namelen);
    char addr[INET_ADDRSTRLEN];
    socklen_t size = sizeof(address);
    inet_ntop(AF_INET, &address.sin_addr, addr, size);
    answer->Set(context, 0, String::NewFromUtf8(isolate, addr, 
      v8::NewStringType::kNormal, strnlen(addr, 255)).ToLocalChecked()).Check();
    answer->Set(context, 1, Integer::New(isolate, 
      ntohs(address.sin_port))).Check();
    args.GetReturnValue().Set(answer);
    return;
  }
  struct sockaddr_un address;
  socklen_t namelen = (socklen_t)sizeof(address);
  int r = getsockname(fd, (struct sockaddr*)&address, &namelen);
  if (r == -1) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    address.sun_path, v8::NewStringType::kNormal, 
    strnlen(address.sun_path, 255)).ToLocalChecked());
}

void just::net::GetPeerName(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int fd = Local<Integer>::Cast(args[0])->Value();
  int domain = Local<Integer>::Cast(args[1])->Value();
  if (domain == AF_INET) {
    Local<Array> answer = args[2].As<Array>();
    struct sockaddr_in address;
    socklen_t namelen = (socklen_t)sizeof(address);
    getpeername(fd, (struct sockaddr*)&address, &namelen);
    char addr[INET_ADDRSTRLEN];
    socklen_t size = sizeof(address);
    inet_ntop(AF_INET, &address.sin_addr, addr, size);
    answer->Set(context, 0, String::NewFromUtf8(isolate, addr, 
      v8::NewStringType::kNormal, strnlen(addr, 255)).ToLocalChecked()).Check();
    answer->Set(context, 1, Integer::New(isolate, 
      ntohs(address.sin_port))).Check();
    args.GetReturnValue().Set(answer);
    return;
  }
  struct sockaddr_un address;
  socklen_t namelen = (socklen_t)sizeof(address);
  int r = getpeername(fd, (struct sockaddr*)&address, &namelen);
  if (r == -1) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, 
    address.sun_path, v8::NewStringType::kNormal, 
    strnlen(address.sun_path, 255)).ToLocalChecked());
}

void just::net::Listen(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  int backlog = Local<Integer>::Cast(args[1])->Value();
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    listen(fd, backlog)));
}

void just::net::SocketPair(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  int domain = Local<Integer>::Cast(args[0])->Value();
  int type = Local<Integer>::Cast(args[1])->Value();
  Local<Array> answer = args[2].As<Array>();
  int fd[2];
  int r = socketpair(domain, type, 0, fd);
  if (r == -1) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  answer->Set(context, 0, Integer::New(isolate, fd[0])).Check();
  answer->Set(context, 1, Integer::New(isolate, fd[1])).Check();
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Pipe(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> answer = args[0].As<Array>();
  int flags = O_CLOEXEC;
  if (args.Length() > 1) flags = Local<Integer>::Cast(args[1])->Value();
  int fd[2];
  int r = pipe2(fd, flags);
  if (r == -1) {
    args.GetReturnValue().Set(Integer::New(isolate, r));
    return;
  }
  answer->Set(context, 0, Integer::New(isolate, fd[0])).Check();
  answer->Set(context, 1, Integer::New(isolate, fd[1])).Check();
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::Connect(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int fd = Local<Integer>::Cast(args[0])->Value();
  int r = 0;
  if (args.Length() > 2) {
    int socktype = AF_INET;
    String::Utf8Value address(isolate, args[1]);
    int port = Local<Integer>::Cast(args[2])->Value();
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
  int fd = Local<Integer>::Cast(args[0])->Value();
  int r = 0;
  if (args.Length() > 2) {
    int socktype = AF_INET;
    String::Utf8Value address(isolate, args[1]);
    int port = Local<Integer>::Cast(args[2])->Value();
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
  int fd = Local<Integer>::Cast(args[0])->Value();
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
  memcpy(buf->GetBackingStore()->Data(), ifr.ifr_hwaddr.sa_data, 6);
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::net::BindInterface(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int fd = Local<Integer>::Cast(args[0])->Value();
  String::Utf8Value name(isolate, args[1]);
  int argc = args.Length();
  int family = AF_PACKET;
  if (argc > 2) family = Local<Integer>::Cast(args[2])->Value();
  int protocol = htons(ETH_P_ALL);
  if (argc > 3) protocol = Local<Integer>::Cast(args[3])->Value();
  int packetType = 0;
  if (argc > 4) packetType = Local<Integer>::Cast(args[4])->Value();
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
  args.GetReturnValue().Set(Integer::New(isolate, 
    bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_ll))));
}

void just::net::Accept(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    accept(Local<Integer>::Cast(args[0])->Value(), NULL, NULL)));
}

void just::net::Accept4(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  int flags = Local<Integer>::Cast(args[1])->Value();
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    accept4(fd, NULL, NULL, flags)));
}

void just::net::Seek(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  int argc = args.Length();
  off_t off = 0;
  if (argc > 1) off = Local<Integer>::Cast(args[1])->Value();
  int whence = SEEK_SET;
  if (argc > 2) whence = Local<Integer>::Cast(args[2])->Value();
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    lseek(fd, off, whence)));
}

void just::net::Read(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  Local<ArrayBuffer> buf = args[1].As<ArrayBuffer>();
  void* data = buf->GetBackingStore()->Data();
  int off = Local<Integer>::Cast(args[2])->Value();
  int len = Local<Integer>::Cast(args[3])->Value();
  const char* dest = (const char*)data + off;
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    read(fd, (void*)dest, len)));
}

void just::net::Recv(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  Local<ArrayBuffer> buf = args[1].As<ArrayBuffer>();
  void* data = buf->GetBackingStore()->Data();
  int off = Local<Integer>::Cast(args[2])->Value();
  int len = Local<Integer>::Cast(args[3])->Value();
  int flags = 0;
  if (args.Length() > 4) flags = Local<Integer>::Cast(args[4])->Value();
  const char* dest = (const char*)data + off;
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    recv(fd, (void*)dest, len, flags)));
}

void just::net::Write(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  void* data = ab->GetBackingStore()->Data();
  int argc = args.Length();
  int len = Local<Integer>::Cast(args[2])->Value();
  int off = 0;
  if (argc > 3) off = Local<Integer>::Cast(args[3])->Value();
  char* buf = (char*)data + off;
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), write(fd, 
    buf, len)));
}

void just::net::Splice(const FunctionCallbackInfo<Value> &args) {
  int infd = Local<Integer>::Cast(args[0])->Value();
  int outfd = Local<Integer>::Cast(args[1])->Value();
  size_t len = Local<BigInt>::Cast(args[2])->Uint64Value();
  Local<Array> offsets = args[3].As<Array>();
  unsigned int flags = Local<Integer>::Cast(args[4])->Value();
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  loff_t inoff = offsets->Get(context, 0).ToLocalChecked()
    .As<BigInt>()->Uint64Value();
  loff_t outoff = offsets->Get(context, 1).ToLocalChecked()
    .As<BigInt>()->Uint64Value();
  ssize_t bytes = splice(infd, &inoff, outfd, &outoff, len, flags);
  if (bytes == -1) {
    args.GetReturnValue().Set(Integer::New(args.GetIsolate(), bytes));
    return;
  }
  offsets->Set(context, 0, BigInt::New(isolate, inoff)).Check();
  offsets->Set(context, 1, BigInt::New(isolate, outoff)).Check();
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), bytes));
}

void just::net::SendFile(const FunctionCallbackInfo<Value> &args) {
  int outfd = Local<Integer>::Cast(args[0])->Value();
  int infd = Local<Integer>::Cast(args[1])->Value();
  size_t count = Local<BigInt>::Cast(args[3])->Uint64Value();
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> offsets = args[2].As<Array>();
  off_t offset = offsets->Get(context, 0).ToLocalChecked()
    .As<BigInt>()->Uint64Value();
  ssize_t bytes = sendfile(outfd, infd, &offset, count);
  if (bytes == -1) {
    args.GetReturnValue().Set(Integer::New(args.GetIsolate(), bytes));
    return;
  }
  offsets->Set(context, 0, BigInt::New(isolate, offset)).Check();
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), bytes));
}

void just::net::WriteString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int fd = Local<Integer>::Cast(args[0])->Value();
  String::Utf8Value str(isolate, args[1]);
  args.GetReturnValue().Set(Integer::New(isolate, write(fd, 
    *str, str.length())));
}

void just::net::Dup2(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  int argc = args.Length();
  if (argc > 1) {
    int fd2 = Local<Integer>::Cast(args[1])->Value();
    args.GetReturnValue().Set(Integer::New(args.GetIsolate(), dup2(fd, fd2)));
    return;
  }
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), dup(fd)));
}

void just::net::Send(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
  void* data = ab->GetBackingStore()->Data();
  int argc = args.Length();
  int len = Local<Integer>::Cast(args[2])->Value();
  int off = 0;
  if (argc > 3) off = Local<Integer>::Cast(args[3])->Value();
  int flags = MSG_NOSIGNAL;
  if (argc > 4) flags = Local<Integer>::Cast(args[4])->Value();
  char* buf = (char*)data + off;
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    send(fd, buf, len, flags)));
}

void just::net::SendString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int fd = Local<Integer>::Cast(args[0])->Value();
  String::Utf8Value str(isolate, args[1]);
  int flags = MSG_NOSIGNAL;
  if (args.Length() > 2) flags = Local<Integer>::Cast(args[2])->Value();
  args.GetReturnValue().Set(Integer::New(isolate, send(fd, 
    *str, str.length(), flags)));
}

void just::net::Close(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), 
    close(Local<Integer>::Cast(args[0])->Value())));
}

void just::net::Shutdown(const FunctionCallbackInfo<Value> &args) {
  int fd = Local<Integer>::Cast(args[0])->Value();
  int how = SHUT_RDWR;
  if (args.Length() > 1) {
    how = Local<Integer>::Cast(args[1])->Value();
  }
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), shutdown(fd, how)));
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
  SET_METHOD(isolate, net, "accept4", Accept4);
  SET_METHOD(isolate, net, "read", Read);
  SET_METHOD(isolate, net, "dup", Dup2);
  SET_METHOD(isolate, net, "seek", Seek);
  SET_METHOD(isolate, net, "recv", Recv);
  SET_METHOD(isolate, net, "write", Write);
  SET_METHOD(isolate, net, "splice", Splice);
  SET_METHOD(isolate, net, "sendfile", SendFile);
  SET_METHOD(isolate, net, "writeString", WriteString);
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
  // NOT AVAILABLE in kernel 4.4
  //SET_VALUE(isolate, net, "AF_KCM", Integer::New(isolate, AF_KCM));
  SET_VALUE(isolate, net, "AF_LLC", Integer::New(isolate, AF_LLC));
  SET_VALUE(isolate, net, "AF_IB", Integer::New(isolate, AF_IB));
  SET_VALUE(isolate, net, "AF_MPLS", Integer::New(isolate, AF_MPLS));
  SET_VALUE(isolate, net, "AF_CAN", Integer::New(isolate, AF_CAN));
  // Protocol Families - bits/socket.h
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
  SET_VALUE(isolate, net, "SO_ATTACH_FILTER", Integer::New(isolate, SO_ATTACH_FILTER));
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
  SET_VALUE(isolate, net, "SHUT_RD", Integer::New(isolate, SHUT_RD));
  SET_VALUE(isolate, net, "SHUT_WR", Integer::New(isolate, SHUT_WR));
  SET_VALUE(isolate, net, "SHUT_RDWR", Integer::New(isolate, SHUT_RDWR));
  SET_MODULE(isolate, target, "net", net);
}
