# distutils: language = c++
cdef extern from "neurons.hpp":
  cdef cppclass neuron_type:
    pass

cdef extern from "neurons.hpp" namespace "neuron_type":
  cdef neuron_type dummy
  cdef neuron_type aqif

cdef extern from "neurons.hpp":
    cppclass Population:
        int n_neurons
        void project()
        Population(int n_neurons, neuron_type nt)

cdef class PyPopulation:

    cdef Population* population_c
    cdef neuron_type nt

    def __init__(self,int n_neurons, str poptype):
        if poptype == "aqif":
            self.nt = <neuron_type> 1
        else:
            self.nt = <neuron_type> 0

        self.population_c = new Population(n_neurons, <neuron_type> 0)
        if self.population_c == NULL:
            raise MemoryError('Not enough memory.')

    @property
    def stupid_attr(self):
        return self._cobj.stupid_attr

    def __del__(self):
        del self._cobj