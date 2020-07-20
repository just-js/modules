const { readFileBytes, writeFile } = require('fs')
const WabtModule = require('./libwabt.js')

const binarySettings = { log: false, write_debug_names: false }
const features = {
  exceptions: false,
  mutable_globals: false,
  sat_float_to_int: false,
  sign_extension: false,
  simd: true,
  threads: false,
  multi_value: false,
  tail_call: false,
  bulk_memory: false,
  reference_types: false
}

function compile (fileName, feat = features, settings = binarySettings) {
  return new Promise((resolve, reject) => {
    WabtModule().then(wabt => {
      var module = wabt.parseWat(fileName, readFileBytes(fileName), feat)
      module.resolveNames()
      module.validate(feat)
      const binaryOutput = module.toBinary(settings)
      resolve({ wasm: binaryOutput.buffer.buffer, source: binaryOutput.log })
    })
  })
}

function createMemory (opts = { initial: 1 }) {
  return new WebAssembly.Memory(opts)
}

function createInstance (mod, context) {
  return new WebAssembly.Instance(mod, context)
}

function evaluate (wasm, context = {}, memory = createMemory()) {
  const mod = new WebAssembly.Module(wasm)
  const js = Object.assign(context, { memory })
  return createInstance(mod, { js }).exports
}

function save (fileName, wasm, flags) {
  return writeFile(fileName, wasm, flags)
}

function load (fileName) {
  return readFileBytes(fileName)
}

module.exports = {
  compile,
  save,
  load,
  evaluate,
  createMemory,
  binarySettings,
  features
}
