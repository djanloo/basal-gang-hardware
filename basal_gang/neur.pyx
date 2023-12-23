# distutils: language = c++
import numpy as np
cimport numpy as np

cimport cython
from libc.stdlib cimport malloc, free
import ctypes

# cdef extern from "neurons.hpp":
#     cdef cppclass dummy_obj:
#         int bro
#         dummy_method()

# cdef class pydummy:

#     cdef dummy_obj * _dummy_obj

#     def __cinit__(self):
#         self._dummy_obj = new dummy_obj()
#         print(self._dummy_obj.bro)

##########################################################
cdef extern from "neurons.hpp":
    cdef cppclass neuron_type:
        pass

cdef extern from "neurons.hpp" namespace "neuron_type":
    cdef neuron_type dummy
    cdef neuron_type aqif

NEURON_TYPES = {"dummy":0, "aqif":1}

#####################################################
cdef extern from "neurons.hpp":
    cdef cppclass Projection:
        int start_dimension, end_dimension
        Projection(double ** memoryview, int start_dimension, int end_dimension)

cdef class PyProjection():
    cdef int start_dimension, end_dimension
    cdef Projection * _projection
    
    def __cinit__(self,  np.ndarray[np.double_t,ndim=2,mode='c'] mat):

        self.start_dimension = mat.shape[0]
        self.end_dimension = mat.shape[1]

        cdef np.ndarray[double, ndim=2, mode="c"] temp_mat = np.ascontiguousarray(mat, dtype = ctypes.c_double)
        cdef double ** mat_pointer = <double **> malloc(self.start_dimension * sizeof(double*))

        if not mat_pointer:
            raise MemoryError

        cdef int i
        for i in range(self.start_dimension):
            mat_pointer[i] = &temp_mat[i, 0]

        self._projection = new Projection(<double **> &mat_pointer[0], self.start_dimension, self.end_dimension)


# #########################################################
cdef extern from "neurons.hpp":
    cdef cppclass Population:
        int n_neurons
        Population(int n_neurons, neuron_type nt)

cdef class PyPopulation:

    cdef Population * _population
    cdef neuron_type _nt

    def __cinit__(self,int n_neurons, str poptype):
        self._nt = <neuron_type><int>NEURON_TYPES[poptype]
        self._population = new Population(<int>n_neurons, self._nt)

    @property
    def n_neurons(self):
        return self._population.n_neurons

    def __dealloc__(self):
        # if self._nt != NULL:
        #     del self._nt
        if self._population != NULL:
            del self._population