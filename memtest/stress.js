const buffers = new Array(1 * 1024)
const { memory } = just.library('memory', '../memory/memory.so')

const size = 64 * 1024

function fill () {
  const len = buffers.length
  for (let i = 0; i < len; i++) {
    buffers[i] = new ArrayBuffer(size)
    //buffers[i] = memory.alloc(64 * 1024)
    //buffers[i] = just.sys.calloc(1, 64 * 1024)
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
