#ifndef JUST_CRYPTO_H
#define JUST_CRYPTO_H

#include <just.h>
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

struct hasher {
  void* src;
  size_t src_len;
	void* dest;
	size_t dest_len;
	EVP_MD_CTX* ctx;
	const EVP_MD* md;
  int handle;
};

std::map<int, hasher*> hashers;
static int hcount = 0;
static bool initialized = false;

void Create(const FunctionCallbackInfo<Value> &args);
void Update(const FunctionCallbackInfo<Value> &args);
void Digest(const FunctionCallbackInfo<Value> &args);
void Reset(const FunctionCallbackInfo<Value> &args);
void Free(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}
}

extern "C" {
	void* _register_crypto() {
		return (void*)just::crypto::Init;
	}
}

#endif
