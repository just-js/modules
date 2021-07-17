const { profiler } = just.library('profiler', './profiler.so')
const r = profiler.snapshot('just.heapsnapshot')
just.print(r)
