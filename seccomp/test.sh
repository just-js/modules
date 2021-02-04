#!/bin/bash
SECCOMP_SYSCALL_ALLOW="" SECCOMP_DEFAULT_ACTION=log just test.js
## do 'dmesg -w' in another terminal to view the audit logs