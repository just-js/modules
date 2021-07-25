const { memtest } = just.library('memtest', './memtest.so')

const size = 64 * 1024
const buffers = new Array(1 * 1024)
const buffer = new ArrayBuffer(size * buffers.length)
just.print(buffer.byteLength)
const buffer2 = new ArrayBuffer(size * buffers.length)
let u8 = new Uint8Array(buffer)
u8.fill(1)
u8 = new Uint8Array(buffer2)
u8.fill(1)

function fill () {
  const len = buffers.length
  for (let i = 0; i < len; i++) {
    //buffers[i] = new ArrayBuffer(size)
    //buffers[i] = memtest.createArrayBuffer(size)
    //buffers[i] = memtest.createExternalBuffer(size)
    //buffers[i] = buffer.slice(0, size)
    //buffers[i] = memtest.createArrayBufferInternal(size)
    //buffers[i] = just.sys.calloc(1, size)
    //buffers[i] = new Uint8Array(buffer)
    //buffers[i] = new Uint8Array(buffer, i * size, size)
    just.sys.memcpy(buffer2, buffer, i * size, size, i * size)
    // These never get freed
    //buffers[i] = memtest.createExternalStaticBuffer(size)
    //buffers[i] = memtest.createBufferObject(size)
  }
  return len
}

let start = Date.now()
let done = 0
while (1) {
  while (Date.now() - start < 1000) done += fill()
  const { rss, external_memory } = just.memoryUsage()
  const elapsed = Date.now() - start
  just.print(`done ${done} elapsed ${elapsed} rate ${Math.floor(done / (elapsed / 1000))} rss ${rss} external ${external_memory}`)
  start = Date.now()
  done = 0
}
