#ifndef JUST_SIGNING_H
#define JUST_SIGNING_H

#include <just.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

namespace just {
namespace signing {

void LoadPublicKey(const FunctionCallbackInfo<Value> &args);
void ExtractRSAPublicKey(const FunctionCallbackInfo<Value> &args);
void LoadCertificate(const FunctionCallbackInfo<Value> &args);
void Verify(const FunctionCallbackInfo<Value> &args);
void LoadPrivateKey(const FunctionCallbackInfo<Value> &args);
void Sign(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}
}

extern "C" {
	void* _register_signing() {
		return (void*)just::signing::Init;
	}
}

#endif
