#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <chrono>

#include "base_objects.hpp"
#include "neurons.hpp"
#include "network.hpp"

using namespace std;

void Axon::fire(EvolutionContext * evo){
    // EFFICIENCY ALERT: this takes too much time, I know
    ((this -> postsynaptic) -> incoming_spikes).push_back(new Spike(this->weight, (evo -> now) + this->delay));
    return;
}

Neuron::Neuron(Population * population){
    this -> state = vector<double> { ((double)rand())/RAND_MAX - 1.0, 0.0, 0.0};
    this -> id = new HierarchicalID( population -> id);

    // TODO: too much redundancy in parameters. 
    // If parameter is the same for the population
    // use a this -> population -> value
    this->E_exc = 0.0;      // mV
    this->E_inh = -80.0;    // mV
    this->E_rest = -60.0;   // mV
    this->E_thr = -50.0;     // mV
    
    this->tau_refrac = 1;  // ms
    this->tau_i = 10;
    this->tau_e = 5;
    this->tau_m = 15;

    this-> last_spike_time = - 1000;

    // Adds to the population
    population -> neurons.push_back(this);
};

void Neuron::connect(Neuron * neuron, double weight, double delay){
    (this -> efferent_axons).push_back(new Axon(this, neuron, weight, delay));
}

void Neuron::evolve(EvolutionContext * evo){
    state[0] += 1.5; // A nice dummy nonsense, impossible to not see this :)
};

void Neuron::spike(EvolutionContext * evo){
    cout << "neuron: " << this->id->local_id << "\tPopulation " << this->id->parent->local_id << " spiked (" << evo->now << ") ms"<< endl;
    for (auto axon : this->efferent_axons){
        (*axon).fire(evo);
    }
    this -> last_spike_time = evo -> now;
    this -> state[0] = this->E_rest;
}

aqif_neuron::aqif_neuron(Population * population) : Neuron(population){
    this -> nt = neuron_type::aqif;
}

void aqif_neuron::evolve(EvolutionContext * evo){

    // Spike processing
    // This could be general
    for (auto spike : this -> incoming_spikes){
        if (!( spike -> processed)){
            if ((spike->arrival_time >= evo->now ) && (spike->arrival_time < evo->now + evo->dt)){
                // Excitatory
                if (spike->weight > 0.0){ 
                    this->state[1] += spike->weight; 
                    spike->processed = true;
                } 
                // Inhibitory
                if (spike->weight < 0.0){ 
                    this->state[2] -= spike->weight; 
                    spike->processed = true;
                }
            }
        }
    }

    // Sub-threshold evolution
    if ( (evo->now) > (this->last_spike_time) + (this->tau_refrac) ){
        // cout << "neuron " << this-> id.local_id  << " of population " << id.parent->local_id << " is evolving " << endl;
        // Membrane decay
        this->state[0] -= ( this->state[0] - this->E_rest) * evo->dt / this->tau_m;
        // Synaptic currents
        this -> state [0] -= evo -> dt * (this -> state [1])*( this -> state[0] - this -> E_exc);
        this -> state [0] -= evo -> dt * (this -> state [2])*( this -> state[0] - this -> E_inh);
    }
    // Synaptic dynamics
    this->state[1] -= (this->state[1]) * (evo->dt) / (this->tau_e);
    this->state[2] -= (this->state[2]) * (evo->dt) / (this->tau_i);

    // Spike generation
    if ((this -> state[0]) > this->E_thr){
        this -> spike(evo);
    }
}



