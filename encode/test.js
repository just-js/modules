const { encode } = just.library('encode', './encode.so')

const { base64Encode, base64Decode, hexEncode, hexDecode } = encode

function testMethod (encode, decode) {
  const buf = ArrayBuffer.fromString('hello')
  const dest = new ArrayBuffer(1024)
  let bytes = encode(buf, dest)
  just.print(bytes)
  just.print(dest.readString(bytes))
  const buf2 = new ArrayBuffer(1024)
  bytes = decode(dest, buf2)
  just.print(bytes)
  just.print(buf2.readString(bytes))
}

testMethod(base64Encode, base64Decode)
testMethod(hexEncode, hexDecode)
