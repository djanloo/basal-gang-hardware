#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include "base_objects.hpp"

using namespace std;

enum class neuron_type : unsigned int {dummy, aqif};

class Spike;
class Axon;
class Neuron;
class Population;
class Projection;

class Spike{
    public:
        Spike(double weight, double arrival_time): weight(weight), arrival_time(arrival_time){
            this -> processed = false;
        }
        bool processed;
        double weight, arrival_time;        
};

struct CompareSpike {
    bool operator()(const Spike * spike1, const Spike * spike2) const {
        return spike1->arrival_time > spike2->arrival_time;
    }
};

class Axon{
    public:
        Axon(Neuron * presynaptic, Neuron * postsynaptic, double weight, double delay):
            presynaptic(presynaptic),postsynaptic(postsynaptic),
            weight(weight), delay(delay){}
            
        void fire(EvolutionContext * evo);
    private:
        Neuron * presynaptic;
        Neuron * postsynaptic;
        double weight, delay;

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
        float E_exc, E_inh, E_rest, E_thr;

        // Spike stuff
        vector<Axon*> efferent_axons;
        priority_queue<Spike*, vector<Spike *>, CompareSpike> incoming_spikes;
        double last_spike_time;

        // Yeah I know, not beautiful but think about it
        // nobody has ever to initialize a neuron outside a population
        Neuron(Population * population); 
        void connect(Neuron * neuron, double weight, double delay);
        void handle_incoming_spikes(EvolutionContext * evo);
        void spike(EvolutionContext * evo);
        void evolve(EvolutionContext * evo);
        vector<double> monitor(){
            return this->state;
        };

        // These must be implemented for each specific neuron
        virtual void evolve_state(EvolutionContext * evo){cout << "WARNING: using virtual evolve_state of <Neuron>";};
        virtual void evolve_synapses(EvolutionContext * evo){cout << "WARNING: using virtual evolve_synapses of <Neuron>";};
};


/*------------------- MORE DETAILED NEURONS ------------------*/
class aqif_neuron : public Neuron {
    public:
        aqif_neuron(Population * population) : Neuron(population){this -> nt = neuron_type::aqif;};

        void evolve_state(EvolutionContext * evo) override; 
        void evolve_synapses(EvolutionContext * evo) override;
};