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
        HierarchicalID * id;
        Population * population;

        // Physiological properties
        float tau_refrac, tau_e, tau_i, tau_m;
        double E_exc, E_inh, E_rest, E_thr;

        // Spike stuff
        vector<Axon*> efferent_axons;
        vector<Spike*> incoming_spikes;
        double last_spike_time;

        // Yeah I know, not beautiful but think about it
        // nobody has ever to initialize a neuron outside a population
        Neuron(Population * population); 
        void connect(Neuron * neuron, double weight, double delay);
        void handle_incoming_spikes(EvolutionContext * evo);
        void spike(EvolutionContext * evo);
        void evolve(EvolutionContext * evo);

        // These must be implemented for each specific neuron
        virtual void evolve_state(EvolutionContext * evo){cout << "WARNING: using virtual evolve_state of <Neuron>";};
        virtual void evolve_synapses(EvolutionContext * evo){cout << "WARNING: using virtual evolve_synapses of <Neuron>";};
};


class aqif_neuron : public Neuron {
    public:
        aqif_neuron(Population * population) : Neuron(population){this -> nt = neuron_type::aqif;};

        void evolve_state(EvolutionContext * evo) override; 
        void evolve_synapses(EvolutionContext * evo) override;
};