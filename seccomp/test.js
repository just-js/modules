const { seccomp } = just.library('seccomp', './seccomp.so')

just.print(seccomp.getNumber('read'))
just.print(seccomp.getNumber('write'))
just.print(seccomp.getNumber('exit'))
just.print(seccomp.getNumber('mknod'))

just.print(seccomp.getName(228))
just.print(seccomp.getName(233))
just.print(seccomp.getName(232))
just.print(seccomp.getName(283))
just.print(seccomp.getName(286))


const syscalls = [
  'read',
  'write',
  'clock_gettime',
  'epoll_ctl',
  'epoll_wait',
  'timerfd_create',
  'timerfd_settime'
]

// allow subset, log on exception
seccomp.allow(syscalls.join(':'), false)

// allow none, log on exception
//seccomp.allow('', true)

// disallow write, log on exception, all others allowed
//seccomp.deny('write', false)

just.setInterval(() => {
  just.print('hello')
}, 1000)

just.setTimeout(() => {
  const fd = just.fs.open('/tmp/foo.txt')
}, 5000)
