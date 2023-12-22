# distutils: language = c++

cdef extern from "neurons.hpp":
    cppclass stupid_class:
        int stupid_attr
    
cdef class PyNeuron:

    cdef stupid_class* _cobj

    def __init__(self):
        self._cobj = new stupid_class()
        if self._cobj == NULL:
            raise MemoryError('Not enough memory.')

    @property
    def stupid_attr(self):
        return self._cobj.stupid_attr

    def __del__(self):
        del self._cobj