const { memtest } = just.library('memtest', './memtest.so')

const size = 64 * 1024
const buffer = memtest.createArrayBufferInternal(size)
const buffer2 = memtest.createBufferObject(size)
const u8 = new Uint8Array(buffer)
u8.fill(32)

const runs = 1024 * 1024

function fill () {
  for (let i = 0; i < runs; i++) {
    //memtest.readArrayBufferFast(buffer2)
    //memtest.readArrayBuffer(buffer)
    memtest.readBufferObject(buffer2)
  }
  return runs
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
