const { signing } = just.library('signing', 'openssl.so')
const { encode } = just.library('encode')

const pubKey = ArrayBuffer.fromString(`
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2xyXizTSTwQt3MUa2vno
qLKPNpYeQzd/pN8BHKcTfjhRxw9Wfi70qFCFrWQP+JE/hNcBWPw6nqC1OuXR74sP
3MfX2S12ooMJWxTBSzok8Ip2WxZ/kvx612HkUmeALbbO3bzI7h1Hsei1VMDeIONr
2oAxjrFwZBbhvcMThYxrtZKWv1X4mgZ7CoQKVZQRhNZZ8nI6cwqGfuUcQ47Nzyir
MKgbVQxCWrgGgv6lOui5o5iCh1HOvfrAMqB418iISkrsPZxrKCs+r0+bwosH41bj
ApgwSEZABteZtsHoPHHoahjLIYf2lNYc5CVywxHSWxhGokF0qwixkjXONdDAE1eT
uwIDAQAB
-----END PUBLIC KEY-----
`)

const privKey = ArrayBuffer.fromString(`
-----BEGIN PRIVATE KEY-----
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDbHJeLNNJPBC3c
xRra+eioso82lh5DN3+k3wEcpxN+OFHHD1Z+LvSoUIWtZA/4kT+E1wFY/DqeoLU6
5dHviw/cx9fZLXaigwlbFMFLOiTwinZbFn+S/HrXYeRSZ4Atts7dvMjuHUex6LVU
wN4g42vagDGOsXBkFuG9wxOFjGu1kpa/VfiaBnsKhApVlBGE1lnycjpzCoZ+5RxD
js3PKKswqBtVDEJauAaC/qU66LmjmIKHUc69+sAyoHjXyIhKSuw9nGsoKz6vT5vC
iwfjVuMCmDBIRkAG15m2weg8cehqGMshh/aU1hzkJXLDEdJbGEaiQXSrCLGSNc41
0MATV5O7AgMBAAECggEAHmcNugGWyuL8EB2DV+4EM+Au0fardU0ul2xgBZ9DK6jZ
5AKDUFfol7AwlDoFo0pYI3EH3VaGNbt72hPIePH1Pf+2ugo+Y1dGRY6QMYV8WaoE
uSfahMUmsFsDaNG2reEnPAo1YSDL8WomOv/V8n9L2FIgTjndq+2m3gNC4q10xoky
NPCjauY2pJuiuPdMc26G3oQF1ljgSgSGh5LWH749dnukGL7wo6oiBmB8R1E/uru1
BdVHvrI0nUgB8z+UKd9yPyydpxHrxso6QnMlIcYNsD/P1/YupeGGZqopATKeikik
Lev+XWqcVuMq6qpTJqPCKKzbSP0al6gA5HUOyDfZuQKBgQD5PKt5dA2lWfoVLztH
sH7Hnwr8ojoDxgpvG0mYp0pE9nhVfRJgQHLdsd0/VXtL+2+neZgnvkYJENZVNY8B
2E+jGJua1aRyO1YhlrPFzoGrTBzZG2UNUCqiy3UyDknwMV79RCI5GGJ7bNPMoNkt
z26MCAjiAtq9Z1rXyo/V6KUYQwKBgQDhDqfz8yds0H9kLabT+MV36wTP7ZgIr66P
mE4OTcPf1mm/5IfmKuoVHDm+v9gCqnEShqS7tI48qD1xc98lTuawbkXG4t+twu03
NO/GtSc2GYGhP+GscOHMP93Q9PKkoIRb0cQKjgFP+vF8wUXr7Qd3cp/ys6eRasHy
u26K1h/7KQKBgQD4bUzOJqtjYagwbX+9GWOFPjYJUvpYwJGt5ZkaG8Wfr7yKGbyz
TAoyiBNsCZx4RAsq97x+AN52Mt4w2M3OnPnkK6cOUl0zr4lOuZQ/WnNNdvRaQ0Aa
YQnecsZCOpPgYpW4BK3IE1I2jGcg1EMlslkSIwAXp5i4qciWx0VPhjDpWQKBgFMX
GiMS7sdZ4u6VfDZebmlrt4cc7TinxAb38DmPtHV7r+dM9G+iAHJyzU8DfTjevIhe
dYtniewTITw1z3jyvHD/BXv8XyDECtiLDsuKLkPm/tkNwL/tnS/PlJGFHeBx8uwD
ciGDdDCXMXkh0jLcfBk5lJwCgCdMBvcbBCD2FTChAoGACiUv80PZP4VraKJBi7F0
Na6vDIRcMZHAK8XgueTTyJwQpRA+QvKjagJpT+ONV7NupQMAcWAfbp51CZ/XgNC3
d/nuUf2/3ZqQtdLxn8/nYm5SUJh+9EgNw7BtxfU/t0z98RQrmjDxmpIA3BkpwpVe
wZ+l/HS/227L4OraUWNEjtg=
-----END PRIVATE KEY-----
`)

const externalSig = 'aaz0p6zQdFgRGXhm/1P2oSbAT/YX2a1cAI8NEou8r8oyvhG3VLKyniznjAgQUtyPBUD5QmzlPNFz/xfC/GUPCReXMmluZuImRxNNVaF4fapa15CVqL0bia8oV6KO7tVCwZcisJXacoab41n6NQUVu9Nkq9+ZFlgite3xiDp5Xa5L1HCuF7qGrlVsz/qLXlCP3z34DTiBkgLxlfhIiyu8mdd+VZpcr6NZB4Ue54hmOejUct6amoL13aXuvJa/Ct8AWL2NaAqrbmsOzik8KJEeSsxgM8UEwvgLF6E+YMtBe1tXhiuH8cVcjNX8py5M0zSPvpouf61fG19gRmS9Q2fhnA=='

// load keys
let rc = signing.RSA.loadPublicKey(pubKey)
if (rc !== 0) throw new Error('Could not load public key')
rc = signing.RSA.loadPrivateKey(privKey)
if (rc !== 0) throw new Error('Could not load private key')

const payload = ArrayBuffer.fromString('My secret message.\n')

// sign the payload with the private key
const sig = new ArrayBuffer(256)
rc = signing.RSA.sign(privKey, payload, sig)
if (rc < 0) throw new Error('Could not sign payload')

const dest = new ArrayBuffer(1024)
const base64 = dest.readString(encode.base64Encode(sig, dest))

// verify the signature with the public key
rc = signing.RSA.verify(pubKey, payload, sig)
if (rc !== 0) throw new Error('Signature is Invalid')

// verify the externally generated signature with the public key
just.print(externalSig)
encode.base64Decode(ArrayBuffer.fromString(externalSig), sig)
rc = signing.RSA.verify(pubKey, payload, sig)
if (rc !== 0) throw new Error('External Signature is Invalid')

// verify the internally generated signature with the public key
just.print(base64)
encode.base64Decode(ArrayBuffer.fromString(base64), sig)
rc = signing.RSA.verify(pubKey, payload, sig)
if (rc !== 0) throw new Error('Internal Signature is Invalid')
