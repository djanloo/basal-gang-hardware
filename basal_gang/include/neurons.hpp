#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "base_objects.hpp"

using namespace std;

enum class neuron_type : unsigned int {dummy, aqif};
typedef  vector<double> neuron_state;

// The menu:
class Spike;
class Synapse;
class Neuron;
class Population;
class Projection;

/*
 * Here a spike is just a weight and an arrival time. 
 * Due to the way they are processed, the natural structure is
 * a priority queue.
 */
class Spike{
    public:
        Spike(double weight, double arrival_time): weight(weight), arrival_time(arrival_time){
            this -> processed = false;
        }
        double weight, arrival_time;
        bool processed;
};

/*
 *  The function used to insert the spike in the queue.
 */
struct CompareSpike {
    bool operator()(const Spike * spike1, const Spike * spike2) const {
        return spike1->arrival_time > spike2->arrival_time;
    }
};
/**
 * The synapse stores the presynaptic and postsynaptic neurons, the weight and the delay.
*/
class Synapse{
    public:
        Synapse(Neuron * presynaptic, Neuron * postsynaptic, double weight, double delay):
            presynaptic(presynaptic),postsynaptic(postsynaptic),
            weight(weight), delay(delay){}
            
        void fire(EvolutionContext * evo);
    private:
        Neuron * presynaptic;
        Neuron * postsynaptic;
        double weight, delay;
};

/**
 * The base dynamical object.
 * The 9 to 5 job of a neuron is:
 *  - process incoming spikes
 *  - evolve the state
 *  - fire if it's the case
 * 
 * The first and the last stage are (roughly) equal for every model,
 * while the evolution equation is model dependent.
 * 
 * To declare a new neuron:
 *  - override the `evolve_state` method
 *  - add the neuron to neuron_type enum class
*/
class Neuron{
    public:
        // Base properties
        vector<double> state; // boost::odeint format, just in case I have to implement it
        neuron_type nt = neuron_type::dummy;
        HierarchicalID * id;
        Population * population;

        // Physiological properties
        float tau_refrac, tau_e, tau_i, tau_m;
        float E_exc, E_inh, E_rest, E_thr;

        // Spike stuff
        vector<Synapse*> efferent_synapses;
        priority_queue<Spike*, vector<Spike *>, CompareSpike> incoming_spikes;
        double last_spike_time;

        // Yeah I know, not beautiful but think about it
        // nobody has ever to initialize a neuron outside a population
        Neuron(Population * population); 
        void connect(Neuron * neuron, double weight, double delay);
        void handle_incoming_spikes(EvolutionContext * evo);
        void spike(EvolutionContext * evo);
        void evolve(EvolutionContext * evo);

        // Monitor function returns the state
        // (Maybe later I will divide V from gsyn, too much data otherwise)
        vector<double> monitor(){
            return this->state;
        };

        // These must be implemented for each specific neuron
        virtual void evolve_state(EvolutionContext * evo){cout << "WARNING: using virtual evolve_state of <Neuron>";};
        virtual void evolve_synapses(EvolutionContext * evo){cout << "WARNING: using virtual evolve_synapses of <Neuron>";};
};


/*------------------- MORE DETAILED NEURONS ------------------*/

/**
 * Still a dummy neuron for now. The name makes sense but the dynamical evolution is completely nonsense.
*/
class aqif_neuron : public Neuron {
    public:
        aqif_neuron(Population * population) : Neuron(population){this -> nt = neuron_type::aqif;};

        // Explicitly override the evolution function
        void evolve_state(EvolutionContext * evo) override; 
};