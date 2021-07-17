const { signing } = just.library('signing', './openssl.so')
const { encode } = just.library('encode')

const pem = `-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA5QqYr0oWQclsyuxTcP80U14hAvYMa7QPcdwQ5BxjggVN47bkiR2Pme9AJnI0EqI8GtfolK7oIajhGJos54B+juENiMEef+1taOgYtrohSy48uBtEvuug0CUkmHS97BtxaWOW7YHgSAbeQmmR6aYJmUxAjyiWVyYkh2TanNLV4O2f55ZpssQXEQliRyzGtY4I3keqxPi4wTp478/Mmkyo3aeyKu4gFuCR5sgRVlIriLcDeiqPbsiQ4BRvOqLDr/gjuqbNbblgDHaLXhWHMfO0ItkiEf0Fe+BmJvk4nHPqWn/+g3EcC5ckTMXbJIpmy86viffST919HiW0qnqgontbtQIDAQAB
-----END PUBLIC KEY-----`
const pubKey = ArrayBuffer.fromString(pem)
let rc = signing.loadPublicKey(pubKey)
if (rc !== 0) throw new Error('Could not load public key')
const payload = ArrayBuffer.fromString('My secret message.\n')
const sig = ArrayBuffer.fromString('qIL+DbhNHjd5iZ0/RonMV1nzfTIwI536FJDiHcu51Mo3c8N+l2W3CYpPOe6fcvdJPkxmID+0UAsBf3dc2GLuEgio2zAjYF/CvS4yC4jIrdal84GiSgX5tEEnhfd+j8dwdiX56uvkbyylYfCFmPwUTnFzQ7aQsQFUFOlA2WTbwl4usYRVSdOKL+QZZlrQvW61/fH3Od+XYVmNvGpwEde/FxZdLfeuuD5ca6hXtBgZpPWxiHnCmw5854RxqNYJn3RwE5BgbNNF57dT9/vdqMAO5PQI/qShNTljY/9dxmslCeVDymO7Dft7UL9WAqsjb2pkHCU2S/i3VH2kMHvQ8C3xzw==')
const signature = new ArrayBuffer(1024)
const bytes = encode.base64Decode(sig, signature)
rc = signing.verify(pubKey, payload, signature.slice(0, bytes))
if (rc !== 0) throw new Error('Signature is Invalid')
