# distutils: language = c++
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

# #########################################################
cdef extern from "neurons.hpp":
    cdef cppclass Population:
        int n_neurons
        Population(int n_neurons, neuron_type nt)

cdef class PyPopulation:

    cdef Population* _population
    cdef neuron_type _nt

    def __init__(self,int n_neurons, str poptype):
        if poptype == "aqif":
            self._nt = <neuron_type> 1
        else:
            self._nt = <neuron_type> 0

        self._population = new Population(<int>n_neurons, self._nt)

    @property
    def n_neurons(self):
        print(f"type of n_neurons is {type(self._population.n_neurons)}")
        return self._population.n_neurons
    
    # @property
    # def pop_address(self):
    #     return self._population

    def __dealloc__(self):
        # if self._nt != NULL:
        #     del self._nt
        if self._population != NULL:
            del self._population