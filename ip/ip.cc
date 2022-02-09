#include "ip.h"

// https://cpp.hotexamples.com/examples/-/-/NLMSG_DATA/cpp-nlmsg_data-function-examples.html#0x44d831afdccb474c1f3c152353533625384b949bae4baa97ad5f05f5119b6428-242,,339,
// https://github.com/aosp-mirror/platform_system_core/blob/master/libnetutils/ifc_utils.c

int string_to_ip(const char *string, struct sockaddr_storage *ss) {
    struct addrinfo hints, *ai;
    int ret;

    if (ss == NULL) {
        return -EFAULT;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_socktype = SOCK_DGRAM;

    ret = getaddrinfo(string, NULL, &hints, &ai);
    if (ret == 0) {
        memcpy(ss, ai->ai_addr, ai->ai_addrlen);
        freeaddrinfo(ai);
    } else {
        // Getaddrinfo has its own error codes. Convert to negative errno.
        // There, the only thing that can reasonably happen is that the passed-in string is invalid.
        ret = (ret == EAI_SYSTEM) ? -errno : -EINVAL;
    }

    return ret;
}

/*
 * Adds or deletes an IP address on an interface.
 *
 * Action is one of:
 * - RTM_NEWADDR (to add a new address)
 * - RTM_DELADDR (to delete an existing address)
 *
 * Returns zero on success and negative errno on failure.
 */
int ifc_act_on_address(int action, const char *name, const char *address,
                       int prefixlen) {
    int ifindex, s, len, ret;
    struct sockaddr_storage ss;
    void *addr;
    size_t addrlen;
    struct {
        struct nlmsghdr n;
        struct ifaddrmsg r;
        // Allow for IPv6 address, headers, and padding.
        char attrbuf[NLMSG_ALIGN(sizeof(struct nlmsghdr)) +
                     NLMSG_ALIGN(sizeof(struct rtattr)) +
                     NLMSG_ALIGN(INET6_ADDRLEN)];
    } req;
    struct rtattr *rta;
    struct nlmsghdr *nh;
    struct nlmsgerr *err;
    char buf[NLMSG_ALIGN(sizeof(struct nlmsghdr)) +
             NLMSG_ALIGN(sizeof(struct nlmsgerr)) +
             NLMSG_ALIGN(sizeof(struct nlmsghdr))];

    // Get interface ID.
    ifindex = if_nametoindex(name);
    if (ifindex == 0) {
        return -errno;
    }

    // Convert string representation to sockaddr_storage.
    ret = string_to_ip(address, &ss);
    if (ret) {
        return ret;
    }

    // Determine address type and length.
    if (ss.ss_family == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in *) &ss;
        addr = &sin->sin_addr;
        addrlen = INET_ADDRLEN;
    } else if (ss.ss_family == AF_INET6) {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) &ss;
        addr = &sin6->sin6_addr;
        addrlen = INET6_ADDRLEN;
    } else {
        return -EAFNOSUPPORT;
    }

    // Fill in netlink structures.
    memset(&req, 0, sizeof(req));

    // Netlink message header.
    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(req.r));
    req.n.nlmsg_type = action;
    req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    req.n.nlmsg_pid = getpid();

    // Interface address message header.
    req.r.ifa_family = ss.ss_family;
    req.r.ifa_prefixlen = prefixlen;
    req.r.ifa_index = ifindex;

    // Routing attribute. Contains the actual IP address.
    rta = (struct rtattr *) (((char *) &req) + NLMSG_ALIGN(req.n.nlmsg_len));
    rta->rta_type = IFA_LOCAL;
    rta->rta_len = RTA_LENGTH(addrlen);
    req.n.nlmsg_len = NLMSG_ALIGN(req.n.nlmsg_len) + RTA_LENGTH(addrlen);
    memcpy(RTA_DATA(rta), addr, addrlen);

    s = socket(PF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE);
    if (send(s, &req, req.n.nlmsg_len, 0) < 0) {
        close(s);
        return -errno;
    }

    len = recv(s, buf, sizeof(buf), 0);
    close(s);
    if (len < 0) {
        return -errno;
    }

    // Parse the acknowledgement to find the return code.
    nh = (struct nlmsghdr *) buf;
    if (!NLMSG_OK(nh, (unsigned) len) || nh->nlmsg_type != NLMSG_ERROR) {
        return -EINVAL;
    }
    err = (nlmsgerr*)NLMSG_DATA(nh);

    // Return code is negative errno.
    return err->error;
}

void just::ip::TunTap(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int fd = Local<Integer>::Cast(args[0])->Value();
  int type = Local<Integer>::Cast(args[1])->Value();
  int flags = Local<Integer>::Cast(args[2])->Value();
  struct ifreq ifr;
  memset(&ifr,0,sizeof(ifr));
  if (args.Length() > 4) {
    String::Utf8Value name(isolate, args[4]);
    strncpy(ifr.ifr_name, *name, sizeof(ifr.ifr_name));
  }
  ifr.ifr_flags = type;
  int r = ioctl(fd, flags, (void*)&ifr);
  if (r == 0 && args.Length() > 3) {
    Local<ArrayBuffer> buf = args[3].As<ArrayBuffer>();
    //strncpy((char*)buf->GetBackingStore()->Data(), ifr.ifr_name, IFNAMSIZ);
    //memcpy(buf->GetBackingStore()->Data(), ifr.ifr_hwaddr.sa_data, 6);
  }
  args.GetReturnValue().Set(Integer::New(isolate, r));
}

void just::ip::SetAddress(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  String::Utf8Value name(isolate, args[0]);
  String::Utf8Value address(isolate, args[1]);
  int prefixLen = Local<Integer>::Cast(args[2])->Value();
  int action = RTM_NEWADDR;
  if (args.Length() > 3) {
    action = Local<Integer>::Cast(args[3])->Value();
  }
  args.GetReturnValue().Set(Integer::New(isolate, ifc_act_on_address(action, *name, *address, prefixLen)));
}

void just::ip::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "tuntap", TunTap);
  SET_METHOD(isolate, module, "setAddress", SetAddress);
  SET_VALUE(isolate, module, "IFF_TUN", Integer::New(isolate, IFF_TUN));
  SET_VALUE(isolate, module, "IFF_TAP", Integer::New(isolate, IFF_TAP));
  SET_VALUE(isolate, module, "TUNSETIFF", Integer::New(isolate, TUNSETIFF));
  SET_VALUE(isolate, module, "TUNGETIFF", Integer::New(isolate, TUNGETIFF));
  SET_VALUE(isolate, module, "IFF_NO_PI", Integer::New(isolate, IFF_NO_PI));
  SET_VALUE(isolate, module, "TUNSETPERSIST", Integer::New(isolate, TUNSETPERSIST));
  SET_VALUE(isolate, module, "TUNSETOWNER", Integer::New(isolate, TUNSETOWNER));
  SET_VALUE(isolate, module, "TUNSETGROUP", Integer::New(isolate, TUNSETGROUP));
  SET_VALUE(isolate, module, "IFNAMSIZ", Integer::New(isolate, IFNAMSIZ));
  SET_VALUE(isolate, module, "SIOCSIFFLAGS", Integer::New(isolate, SIOCSIFFLAGS));
  SET_VALUE(isolate, module, "IFF_UP", Integer::New(isolate, IFF_UP));
  SET_VALUE(isolate, module, "IFF_BROADCAST", Integer::New(isolate, IFF_BROADCAST));
  SET_VALUE(isolate, module, "IFF_LOOPBACK", Integer::New(isolate, IFF_LOOPBACK));
  SET_VALUE(isolate, module, "IFF_MULTICAST", Integer::New(isolate, IFF_MULTICAST));
  SET_VALUE(isolate, module, "IFF_PROMISC", Integer::New(isolate, IFF_PROMISC));
  SET_VALUE(isolate, module, "IFF_NOARP", Integer::New(isolate, IFF_NOARP));
  SET_VALUE(isolate, module, "RTM_NEWADDR", Integer::New(isolate, RTM_NEWADDR));
  SET_VALUE(isolate, module, "RTM_DELADDR", Integer::New(isolate, RTM_DELADDR));

  SET_MODULE(isolate, target, "ip", module);
}
