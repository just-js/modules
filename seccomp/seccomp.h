#ifndef JUST_ZLIB_H
#define JUST_ZLIB_H

#include "just.h"
#include <seccomp.h>

/*
code lifted from here: git@github.com:cloudflare/sandbox.git

BSD 3-Clause License

Copyright (c) 2020, Cloudflare
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

namespace just {

namespace seccomp {

#define SYSCALL_NAME_MAX_LEN 128
#define SECCOMP_SYSCALL_ALLOW "SECCOMP_SYSCALL_ALLOW"
#define SECCOMP_SYSCALL_DENY "SECCOMP_SYSCALL_DENY"
#define SECCOMP_DEFAULT_ACTION "SECCOMP_DEFAULT_ACTION"

static const char *get_seccomp_action_name(uint32_t action)
{
  const char *action_name;
  switch (action) {
    case SCMP_ACT_KILL_PROCESS:
      action_name = "kill process";
      break;
    case SCMP_ACT_LOG:
      action_name = "log then allow";
      break;
    case SCMP_ACT_ALLOW:
      action_name = "allow";
      break;
    default:
      /* TODO(paulsmith): add remaining seccomp action types
          when they are supported */
      action_name = "unknown action";
  }
  return action_name;
}

static void add_syscall(scmp_filter_ctx ctx, const char *syscall, uint32_t action)
{
	int syscall_nr = seccomp_syscall_resolve_name(syscall);
	if (__NR_SCMP_ERROR == syscall_nr) {
		fprintf(stderr, "failed to find the syscall number for %s\n", syscall);
		seccomp_release(ctx);
		exit(1);
	}

	if (seccomp_rule_add_exact(ctx, action, syscall_nr, 0)) {
		fprintf(stderr, "failed to add %s to the seccomp filter context\n", syscall);
		seccomp_release(ctx);
		exit(1);
	}
}

void Allow(const FunctionCallbackInfo<Value> &args);
void Deny(const FunctionCallbackInfo<Value> &args);
void GetName(const FunctionCallbackInfo<Value> &args);
void GetNumber(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_seccomp() {
		return (void*)just::seccomp::Init;
	}
}

#endif
