#ifndef JUST_NET_H
#define JUST_NET_H

#include <just.h>

extern "C" {
#define class xclass
#include <kvm/ioport.h>
#include <kvm/term.h>
#undef class
}

#include <linux/types.h>
#include <linux/err.h>

#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

__thread struct kvm_cpu *current_kvm_cpu;
#define MB_SHIFT		(20)

bool do_debug_print = false;

namespace just {

namespace jkvm {


void Create(const FunctionCallbackInfo<Value> &args);
void CreateSync(const FunctionCallbackInfo<Value> &args);
void Destroy(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_jkvm() {
		return (void*)just::jkvm::Init;
	}
}

#endif
