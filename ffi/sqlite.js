/*
const ref = require('ref')
const ffi = require('../')

const dbName = process.argv[2] || 'test.sqlite3'
const sqlite3 = 'void'
const sqlite3Ptr = ref.refType(sqlite3)
const sqlite3PtrPtr = ref.refType(sqlite3Ptr)
const sqlite3_exec_callback = 'pointer'
const stringPtr = ref.refType('string')
const SQLite3 = ffi.Library('libsqlite3', {
  sqlite3_libversion: ['string', []],
  sqlite3_open: ['int', ['string', sqlite3PtrPtr]],
  sqlite3_close: ['int', [sqlite3Ptr]],
  sqlite3_changes: ['int', [sqlite3Ptr]],
  sqlite3_exec: ['int', [sqlite3Ptr, 'string', sqlite3_exec_callback, 'void *', stringPtr]]
})
console.log('Using libsqlite3 version %j...', SQLite3.sqlite3_libversion())
var db = ref.alloc(sqlite3PtrPtr)
console.log('Opening %j...', dbName)
SQLite3.sqlite3_open(dbName, db)
db = db.deref()
console.log('Creating and/or clearing foo table...')
SQLite3.sqlite3_exec(db, 'CREATE TABLE foo (bar VARCHAR);', null, null, null)
SQLite3.sqlite3_exec(db, 'DELETE FROM foo;', null, null, null)
console.log('Inserting bar 5 times...')
for (var i = 0; i < 5; i++) {
  SQLite3.sqlite3_exec(db, 'INSERT INTO foo VALUES(\'baz' + i + '\');', null, null, null)
}
*/

const { ffi } = just.library('./ffi.so', 'ffi')
const handle = just.sys.dlopen('/usr/lib/x86_64-linux-gnu/libsqlite3.so')
if (!handle) throw new Error('Clould not create handle')

function strlen (ptr) {
  const fn = just.sys.dlsym(handle, 'strlen')
  if (!fn) throw new Error('Could not find symbol')
  const cif = new ArrayBuffer(8)
  const dv = new DataView(cif)
  const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_UINT32, [ffi.FFI_TYPE_POINTER])
  if (status !== ffi.FFI_OK) {
    throw new Error(`Bad Status ${status}`)
  }
  dv.setBigUint64(0, ptr, true)
  return ffi.ffiCall(cif, fn)
}

function version () {
  const fn = just.sys.dlsym(handle, 'sqlite3_libversion')
  if (!fn) throw new Error('Could not find symbol')
  const cif = new ArrayBuffer(8)
  const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_POINTER, [])
  if (status !== ffi.FFI_OK) {
    throw new Error(`Bad Status ${status}`)
  }
  const ptr = ffi.ffiCall(cif, fn)
  const len = strlen(ptr)
  return just.sys.readMemory(ptr, ptr + BigInt(len)).readString(len)
}

function open (fileName) {
  const fn = just.sys.dlsym(handle, 'sqlite3_open')
  if (!fn) throw new Error('Could not find symbol')
  const cif = new ArrayBuffer(16)
  const dv = new DataView(cif)
  const db2 = new ArrayBuffer(1024)
  const db = new ArrayBuffer(8) // pointer for db handle
  const dv2 = new DataView(db)
  dv2.setBigUint64(0, db2.getAddress(), true)
  const status = ffi.ffiPrepCif(cif, ffi.FFI_TYPE_UINT32, [ffi.FFI_TYPE_POINTER, ffi.FFI_TYPE_POINTER])
  if (status !== ffi.FFI_OK) {
    throw new Error(`Bad Status ${status}`)
  }
  const buf = ArrayBuffer.fromString(fileName)
  dv.setBigUint64(0, buf.getAddress(), true)
  dv.setBigUint64(8, db.getAddress(), true)
  const r = ffi.ffiCall(cif, fn)
  just.print(r)
  return db
}

just.print(version())
const r = open(':memory:')
just.print(r)
