#include <iostream>
#include <vector>
#include <list>

using namespace std;

enum neuron_type {dummy, aqif};

class stupid_class{
    public:
        int stupid_attr;
};


// Neurons stuff
class Neuron{
    public:
        vector<double> state;
        int state_dimension;

        vector<Neuron*> childs;
        vector<double> spike_times;
        neuron_type nt = dummy;

        Neuron();
        virtual void evolve();
        virtual void connect(Neuron * neuron);
        virtual void spike();
};

// Specific neuron stuff
class aqif_neuron : public Neuron {
    public:
        aqif_neuron();
        void evolve();
};

// Populations stuff
class Population{
    public:
        vector<Neuron*> neurons;
        int n_neurons;
        Population(int n_neurons, neuron_type nt);
        void project(Population * child_pop);
        void evolve();
};
