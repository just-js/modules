const { ffi } = just.library('./ffi.so', 'ffi')
const handle = just.sys.dlopen()
if (!handle) throw new Error('Clould not create handle')

function atoi (str) {
  const fn = just.sys.dlsym(handle, 'atoi')
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

just.print(atoi(just.args[2] || '1'))
just.print(chdir(just.args[3] || '/tmp'))
const fd = just.fs.open('node.cc')
just.print(fd)
just.print(close(fd))
