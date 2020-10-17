const { ffi } = just.library('./ffi.so', 'ffi')
const handle = just.sys.dlopen()
if (!handle) throw new Error('Clould not create handle')

function chdir (str) {
  const fn = just.sys.dlsym(handle, 'chdir')
  if (!fn) throw new Error('Could not find symbol')
  const cif = new ArrayBuffer(8)
  const dv = new DataView(cif)
  const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_UINT32, [ffi.FFI_TYPE_POINTER])
  if (status !== ffi.FFI_OK) {
    throw new Error(`Bad Status ${status}`)
  }
  const buf = ArrayBuffer.fromString(str)
  dv.setBigUint64(0, buf.getAddress(), true)
  return ffi.ffiCall(cif, fn)
}

function close (fd) {
  const fn = just.sys.dlsym(handle, 'close')
  if (!fn) throw new Error('Could not find symbol')
  const cif = new ArrayBuffer(4)
  const dv = new DataView(cif)
  const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_UINT32, [ffi.FFI_TYPE_UINT32])
  if (status !== ffi.FFI_OK) {
    throw new Error(`Bad Status ${status}`)
  }
  dv.setUint32(0, fd, true)
  return ffi.ffiCall(cif, fn)
}

function wrap_atoi () {
  const fn = just.sys.dlsym(handle, 'atoi')
  if (!fn) throw new Error('Could not find symbol')
  const cif = new ArrayBuffer(8)
  const dv = new DataView(cif)
  const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_UINT32, [ffi.FFI_TYPE_POINTER])
  if (status !== ffi.FFI_OK) {
    throw new Error(`Bad Status ${status}`)
  }
  return str => {
    const buf = ArrayBuffer.fromString(str)
    dv.setBigUint64(0, buf.getAddress(), true)
    return ffi.ffiCall(cif, fn)
  }
}

function wrap_uname () {
  const fn = just.sys.dlsym(handle, 'uname')
  if (!fn) throw new Error('Could not find symbol')
  const cif = new ArrayBuffer(8)
  const dv = new DataView(cif)
  const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_UINT32, [ffi.FFI_TYPE_POINTER])
  if (status !== ffi.FFI_OK) {
    throw new Error(`Bad Status ${status}`)
  }
  return buf => {
    dv.setBigUint64(0, buf.getAddress(), true)
    return ffi.ffiCall(cif, fn)
  }
}

function wrap_sqrt () {
  const fn = just.sys.dlsym(handle, 'sqrt')
  if (!fn) throw new Error('Could not find symbol')
  const cif = new ArrayBuffer(8)
  const dv = new DataView(cif)
  const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_DOUBLE, [ffi.FFI_TYPE_DOUBLE])
  if (status !== ffi.FFI_OK) {
    throw new Error(`Bad Status ${status}`)
  }
  return val => {
    dv.setFloat64(0, val, true)
    return ffi.ffiCall(cif, fn)
  }
}

function readCString (buf, u8, off) {
  const start = off
  while (u8[off] !== 0) off++
  return buf.readString(off - start, start)
}

const atoi = wrap_atoi()
just.print(atoi(just.args[2] || '1'))
just.print(atoi(just.args[2] || '22'))
just.print(atoi(just.args[2] || '333'))
just.print(atoi(just.args[2] || '4444'))

const uname = wrap_uname()
const b = new ArrayBuffer(4096)
const u8 = new Uint8Array(b)
uname(b)
const _UTSNAME_LENGTH = 65
const sysname = readCString(b, u8, 0)
just.print(sysname)
const nodename = readCString(b, u8, _UTSNAME_LENGTH)
just.print(nodename)
const release = readCString(b, u8, _UTSNAME_LENGTH * 2)
just.print(release)
const version = readCString(b, u8, _UTSNAME_LENGTH * 3)
just.print(version)
const machine = readCString(b, u8, _UTSNAME_LENGTH * 4)
just.print(machine)
const domain = readCString(b, u8, _UTSNAME_LENGTH * 5)
just.print(domain)

const sqrt = wrap_sqrt()
just.print(sqrt(9.0))

just.print(chdir(just.args[3] || '/tmp'))
const fd = just.fs.open('just.cc')
just.print(fd)
just.print(close(fd))
