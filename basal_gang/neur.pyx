# distutils: language = c++
import os
from libc.stdlib cimport malloc, free
import ctypes

import numpy as np
cimport numpy as np




NEURON_TYPES = {"dummy":0, "aqif":1}

cdef extern from "include/base_objects.hpp":
    cdef cppclass EvolutionContext:
        double dt
        double now
        EvolutionContext(double _dt)

    cdef cppclass HierarchicalID:
        pass

cdef extern from "include/devices.hpp":
    cdef cppclass Monitor[T]:
        Monitor(T * object)
        void gather()
        get_history()
    
    cdef cppclass Injector[var]:
        Injector(var * variable, double rate, double t_max)
        void inject()

cdef extern from "include/neurons.hpp":
    cdef cppclass neuron_type:
        pass        

cdef extern from "include/neurons.hpp" namespace "neuron_type":
    cdef neuron_type dummy
    cdef neuron_type aqif


cdef extern from "include/network.hpp":
    cdef cppclass Projection:
        int start_dimension, end_dimension
        Projection(double ** weights, double ** delays, int start_dimension, int end_dimension)

    cdef cppclass Population:
        int n_neurons
        HierarchicalID * id

        int n_spikes_last_step

        Population(int n_neurons, neuron_type nt, SpikingNetwork * spiking_network)
        void project(Projection * projection, Population * child_pop)
        void evolve(EvolutionContext * evo)

    cdef cppclass SpikingNetwork:
        HierarchicalID * id
        SpikingNetwork()
        void run(EvolutionContext * evo, double time)


cdef class PyProjection():

    cdef int start_dimension, end_dimension
    cdef double ** _weights
    cdef double ** _delays 
    cdef Projection * _projection

    cdef double [:,:] weights, delays

    def __cinit__(self,  np.ndarray[np.double_t,ndim=2,mode='c'] weights, np.ndarray[np.double_t,ndim=2,mode='c'] delays):
        self.weights = weights
        self.delays = delays

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
    @property
    def weights(self):
        return self.weights
    
    @property
    def delays(self):
        return self.delays


cdef class PyPopulation:

    cdef Population * _population
    cdef neuron_type _nt
    cdef Monitor[Population] * _monitor 

    def __cinit__(self, int n_neurons, str poptype, PySpikingNetwork spikenet):
        self._nt = <neuron_type><int>NEURON_TYPES[poptype]
        self.spikenet = spikenet
        self._population = new Population(<int>n_neurons, self._nt, spikenet._spiking_network)
        self._monitor = NULL

    @property
    def n_neurons(self):
        return self._population.n_neurons

    @property
    def n_spikes_last_step(self):
        return self._population.n_spikes_last_step

    def project(self, PyProjection proj, PyPopulation efferent_pop):
        self._population.project(proj._projection, efferent_pop._population)

    def monitorize(self):
        self._monitor = new Monitor[Population](self._population)
    
    def get_data(self):
        if self._monitor:
            return self._monitor.get_history()

    def __dealloc__(self):
        if self._population != NULL:
            del self._population

cdef class PySpikingNetwork:

    cdef SpikingNetwork * _spiking_network
    cdef EvolutionContext * evo
    cdef str name

    def __cinit__(self, str name):
        self._spiking_network = new SpikingNetwork()
        self.name = name

    def run(self, dt=0.1, time=1):
        self.evo = new EvolutionContext(dt)
        self._spiking_network.run(self.evo, time)


    @classmethod
    def from_yaml(cls, yaml_file):
        net = cls("Albert")

        net.yaml_file = yaml_file
        
        if not os.path.exists(net.yaml_file):
            raise FileNotFoundError("YAML file not found")
        
        with open(net.yaml_file, "r") as f:
            net.features_dict = net.safe_load(f)

        net.populations = dict()
        
        for pop in net.features_dict['populations']:
            net.populations[pop['name']] = PyPopulation(pop['size'], pop['neuron_type'], net)

        for proj in net.features_dict['projections']:
            projector = RandomProjector(**(proj['features']))
            efferent = net.populations[proj['efferent']]
            afferent = net.populations[proj['afferent']]
            efferent.project(projector.get_projection(efferent, afferent), afferent)


class RandomProjector:

    def __init__(self,  inh_fraction=0.0, exc_fraction=0.0, 
                        max_inh = 0.1, max_exc=0.1, 
                        min_delay=0.1, max_delay=0.5):
        assert max_inh > 0, "AAAAA"
        self.max_inh = max_inh
        self.max_exc = max_exc
        self.exc_fraction = exc_fraction
        self.inh_fraction = inh_fraction
        self.min_delay = min_delay
        self.max_delay = max_delay

    def get_projection(self, PyPopulation pop1, PyPopulation pop2):

        N, M = pop1.n_neurons, pop2.n_neurons

        active_inh_syn = (np.random.uniform(0,1, size=(N,M)) < self.inh_fraction)
        active_exc_syn = (np.random.uniform(0,1, size=(N,M)) < self.exc_fraction)

        weights = np.zeros((N,M))
        delays = np.zeros((N,M))

        for i in range(N):
            for j in range(M):
                if active_inh_syn[i,j]:
                    weights[i,j] = -np.random.uniform(0,self.max_inh)
                    delays[i,j] = np.random.uniform(self.min_delay, self.max_delay)
                elif active_exc_syn[i,j]:
                    weights[i,j] = np.random.uniform(0, self.max_exc)
                    delays[i,j] = np.random.uniform(self.min_delay, self.max_delay)

 
        self.last_weights = weights
        self.last_delays = delays

        self.last_projection = PyProjection(weights, delays)
        return self.last_projection
