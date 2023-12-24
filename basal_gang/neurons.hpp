#pragma once
#include <iostream>
#include <vector>
#include <list>
#include "base_objects.hpp"

using namespace std;

enum class neuron_type : unsigned int {dummy, aqif};

class Axon;
class Neuron;
class Population;
class Projection;

class Axon{
    public:
        Neuron * presynaptic;
        Neuron * postsynaptic;
        double weight, delay;

        Axon(Neuron * _presynaptic, Neuron * _postsynaptic, double _weight, double _delay){
            this -> presynaptic = _presynaptic;
            this -> postsynaptic = _postsynaptic;
            this -> weight = _weight;
            this -> delay = _delay;
        }
        void fire(EvolutionContext * evo);
};

class Neuron{
    public:
        // Base properties
        vector<double> state;
        neuron_type nt = neuron_type::dummy;
        int index;

        // Physiological properties
        float tau_refrac, tau_e, tau_i, tau_m;
        double E_exc, E_inh, E_rest, E_thr;

        // Spike stuff
        vector<Axon*> efferent_axons;
        vector<Spike*> incoming_spikes;
        double last_spike_time;

        Neuron(int _index);
        void connect(Neuron * neuron, double weight, double delay);
        void spike(EvolutionContext * evo);
        virtual void evolve(EvolutionContext * evo);
};


class aqif_neuron : public Neuron {
    public:
        aqif_neuron(int _index);
        void evolve(EvolutionContext * evo);
};