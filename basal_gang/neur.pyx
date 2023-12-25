# distutils: language = c++
import numpy as np
cimport numpy as np

from libc.stdlib cimport malloc, free
import ctypes

cdef extern from "base_objects.hpp":
    cdef cppclass EvolutionContext:
        double dt
        double now
        EvolutionContext()

    cdef cppclass HierarchicalID:
        pass

cdef extern from "neurons.hpp":
    cdef cppclass neuron_type:
        pass

cdef extern from "neurons.hpp" namespace "neuron_type":
    cdef neuron_type dummy
    cdef neuron_type aqif

NEURON_TYPES = {"dummy":0, "aqif":1}

cdef extern from "network.hpp":
    cdef cppclass Projection:
        int start_dimension, end_dimension
        Projection(double ** weights, double ** delays, int start_dimension, int end_dimension)

cdef class PyProjection():

    cdef int start_dimension, end_dimension
    cdef double ** _weights
    cdef double ** _delays 
    cdef Projection * _projection
    
    def __cinit__(self,  np.ndarray[np.double_t,ndim=2,mode='c'] weights, np.ndarray[np.double_t,ndim=2,mode='c'] delays):

        self.start_dimension = weights.shape[0]
        self.end_dimension   = weights.shape[1]

        cdef np.ndarray[double, ndim=2, mode="c"] contiguous_weights = np.ascontiguousarray(weights, dtype = ctypes.c_double)
        cdef np.ndarray[double, ndim=2, mode="c"] contiguous_delays = np.ascontiguousarray(delays, dtype = ctypes.c_double)

        self._weights = <double **> malloc(self.start_dimension * sizeof(double*))
        self._delays  = <double **> malloc(self.start_dimension * sizeof(double*))

        if not self._weights or not self._delays:
            raise MemoryError

        cdef int i
        for i in range(self.start_dimension):
            self._weights[i] = &contiguous_weights[i, 0]
            self._delays[i] = &contiguous_delays[i,0]

        self._projection = new Projection(  <double**> &self._weights[0], 
                                            <double**> &self._delays[0], 
                                            self.start_dimension, 
                                            self.end_dimension)

cdef extern from "network.hpp":
    cdef cppclass Population:
        int n_neurons
        Population(int n_neurons, neuron_type nt, HierarchicalID * spikenet_id)

cdef class PyPopulation:

    cdef Population * _population
    cdef neuron_type _nt

    def __cinit__(self, int n_neurons, str poptype, PySpikingNetwork spikenet):
        self._nt = <neuron_type><int>NEURON_TYPES[poptype]
        self._population = new Population(<int>n_neurons, self._nt, &spikenet._spiking_network.id)

    @property
    def n_neurons(self):
        return self._population.n_neurons

    def __dealloc__(self):
        if self._population != NULL:
            del self._population


cdef extern from "network.hpp":
    cdef cppclass SpikingNetwork:
        HierarchicalID id
        void add_population(Population * pop)
        run(EvolutionContext * evo, double time) 

cdef class PySpikingNetwork:

    cdef SpikingNetwork * _spiking_network
    cdef str name
    def __cinit__(self, str name):
        self._spiking_network = new SpikingNetwork()
        self.name = name

    cdef add_population(self, PyPopulation pypop):
        (self._spiking_network).add_population( pypop._population )
