const { ffi } = just.library('./ffi.so', 'ffi')
const handle = just.sys.dlopen()
if (!handle) throw new Error('Clould not create handle')
const fn = just.sys.dlsym(handle, 'atoi')
if (!fn) throw new Error('Could not find symbol')
const cif = new ArrayBuffer(8)
const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_UINT32, [ffi.FFI_TYPE_POINTER])
if (status !== ffi.FFI_OK) {
  throw new Error(`Bad Status ${status}`)
}
just.print(ffi.ffiCall(cif, fn, just.args[2] || '1'))
