#ifndef JUST_CRYPTO_H
#define JUST_CRYPTO_H

#include "just.h"
#include <openssl/opensslv.h>
#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

namespace just {
namespace crypto {

void Hash(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}
}

extern "C" {
	void* _register_crypto() {
		return (void*)just::crypto::Init;
	}
}

#endif
