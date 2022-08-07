const { fs } = just.library('fs')
const { net } = just.library('net')
const { iouring } = just.library('iouring', './iouring.so')

const { SystemError } = just
const { EAGAIN } = net

const RINGSIZE = 64
const BLOCKSIZE	= 128 * 1024
const buffers = new Array(Number(RINGSIZE)).fill(0).map(e => new ArrayBuffer(Number(BLOCKSIZE)))

//just.print = () => {}

const ring = iouring.queueInit(RINGSIZE)
if (!ring) throw new SystemError('iouring.queueInit')

let fd = just.sys.STDIN_FILENO
if (just.args.length > 2) {
  const fileName = just.args[2] || '/dev/shm/in.bin'
  fd = fs.open(fileName, fs.O_RDONLY)
  if (fd < 0) throw new SystemError('open file')
}

let offset = 0
const sqe = iouring.getSQE(ring)
if (!sqe) throw new SystemError('iouring.getSQE')
iouring.prepReadV(sqe, fd, buffers, offset)
const r = iouring.submit(ring)
just.print(`iouring.submit ${r}`)

let total = 0
let chunks = 0
const res = [0, 0]
iouring.waitCQE(ring, res)
const [ret, cqe] = res
just.print(`waitCQE ret ${ret}`)
let completion = cqe

while (completion) {
  iouring.getData(completion, res)
  const [bytes, index] = res
  just.print(`getData bytes ${bytes} index ${index} ${offset}`)
  if (bytes > 0) {
    offset += bytes
    total += bytes
    chunks++
    //just.error(buffers[index].readString(bytes))
  }
  if (bytes === 0) break
  iouring.cqeSeen(ring, completion)
  iouring.peekCQE(ring, res)
  const [ret, cqe] = res
  just.print(`peekCQE ret ${ret}`)
  if (ret === -EAGAIN) {
    const submission = iouring.getSQE(ring)
    if (!submission) throw new SystemError('iouring.getSQE')
    iouring.prepReadV(submission, fd, buffers, offset)
    const r = iouring.submit(ring)
    just.print(`iouring.submit ${r}`)
    iouring.waitCQE(ring, res)
    const [ret, cqe] = res
    just.print(`waitCQE ret ${ret}`)
    completion = cqe
  } else {
    completion = cqe
  }
}
iouring.queueExit(ring)
just.error(`size ${total} chunks ${chunks}`)
