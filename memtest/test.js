const { memtest } = just.library('memtest', './memtest.so')

const buf = new ArrayBuffer(16384)

function test (count = 10000000, fn) {
  const start = Date.now()
  for (let i = 0; i < count; i++) {
    fn()
  }
  const elapsed = (Date.now() - start) / 1000
  const rate = (count / elapsed)
  just.print(`count ${count} in ${elapsed.toFixed(2)} rate ${rate.toFixed(2)} rss ${just.memoryUsage().rss}`)
}

function testall () {
  for (let i = 0; i < 5; i++) {
    test(50000000, () => memtest.readOne(buf, 16384))
  }
  for (let i = 0; i < 5; i++) {
    test(10000000, () => memtest.readTwo(buf))
  }
  just.setTimeout(testall, 1000)
}

testall()
