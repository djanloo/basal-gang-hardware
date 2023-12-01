def dummy_func():
  cdef int i
  cdef float a = 0
  for i in range(10000):
    a += 1.2
  return a
