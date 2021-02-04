const { seccomp } = just.library('seccomp', './seccomp.so')

seccomp.createFilter()

just.print(seccomp.getNumber('read'))
just.print(seccomp.getNumber('write'))
just.print(seccomp.getNumber('exit'))
just.print(seccomp.getNumber('mknod'))

just.print(seccomp.getName(233))
just.print(seccomp.getName(232))
just.print(seccomp.getName(283))
just.print(seccomp.getName(286))

just.setInterval(() => {
  just.print('hello')
}, 1000)
