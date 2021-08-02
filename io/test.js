const { io } = just.library('io', './io.so')
const FC_MAGIC_IOPORT = 0x03f0
const FC_MAGIC_IOPORT_VALUE = 123
let err = io.iopl(3)
if (err < 0) throw new just.SystemError('iopl')
err = io.ioperm(FC_MAGIC_IOPORT_VALUE, 16n, 1)
if (err < 0) throw new just.SystemError('ioperm')
io.outb(FC_MAGIC_IOPORT_VALUE, FC_MAGIC_IOPORT)
