#ifndef JUST_NET_H
#define JUST_NET_H

#include <just.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>

namespace just {

namespace net {
void Socket(const FunctionCallbackInfo<Value> &args);
void SetSockOpt(const FunctionCallbackInfo<Value> &args);
void GetSockOpt(const FunctionCallbackInfo<Value> &args);
void GetSockName(const FunctionCallbackInfo<Value> &args);
void GetPeerName(const FunctionCallbackInfo<Value> &args);
void Listen(const FunctionCallbackInfo<Value> &args);
void SocketPair(const FunctionCallbackInfo<Value> &args);
void Pipe(const FunctionCallbackInfo<Value> &args);
void Connect(const FunctionCallbackInfo<Value> &args);
void Bind(const FunctionCallbackInfo<Value> &args);
void BindInterface(const FunctionCallbackInfo<Value> &args);
void GetMacAddress(const FunctionCallbackInfo<Value> &args);
void Accept(const FunctionCallbackInfo<Value> &args);
void Seek(const FunctionCallbackInfo<Value> &args);
void Read(const FunctionCallbackInfo<Value> &args);
void Recv(const FunctionCallbackInfo<Value> &args);
void Write(const FunctionCallbackInfo<Value> &args);
void WriteString(const FunctionCallbackInfo<Value> &args);
void Writev(const FunctionCallbackInfo<Value> &args);
void Send(const FunctionCallbackInfo<Value> &args);
void SendString(const FunctionCallbackInfo<Value> &args);
void Close(const FunctionCallbackInfo<Value> &args);
void Shutdown(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_net() {
		return (void*)just::net::Init;
	}
}

#endif
