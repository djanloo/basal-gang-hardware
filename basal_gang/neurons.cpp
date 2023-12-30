#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <chrono>

#include "include/base_objects.hpp"
#include "include/devices.hpp"
#include "include/neurons.hpp"
#include "include/network.hpp"

void Synapse::fire(EvolutionContext * evo){
    Spike * newspike = new Spike(this->weight, evo->now + this->delay);
    this->postsynaptic->incoming_spikes.push(newspike);
    return;
}

Neuron::Neuron(Population * population){
    // TODO: too much redundancy in parameters. 
    // If parameter is the same for the population
    // use a this -> population -> value
    this->E_exc = 0.0;      // mV
    this->E_inh = -80.0;    // mV
    this->E_rest = -60.0;   // mV
    this->E_thr = -40.0;     // mV
    
    this->tau_refrac = 1;  // ms
    this->tau_i = 10;
    this->tau_e = 5;
    this->tau_m = 15;

    this -> state = vector<double> { 
                                    this -> E_rest + ((double)rand())/RAND_MAX, 
                                    0.0, 
                                    0.0
                                    };


    this -> id = new HierarchicalID( population -> id);
    this -> population = population;

    this-> last_spike_time = - 1000;

    // Adds to the population
    population -> neurons.push_back(this);
};

void Neuron::connect(Neuron * neuron, double weight, double delay){
    (this -> efferent_synapses).push_back(new Synapse(this, neuron, weight, delay));
}

void Neuron::handle_incoming_spikes(EvolutionContext * evo){

    Spike * spike;

    while (!(this->incoming_spikes.empty())){
        
        spike = this->incoming_spikes.top();

        if ((spike->arrival_time < evo->now)&(!spike->processed)){cout << "ERROR: spike missed" << endl;} 

        if (!(spike -> processed)){

            if ((spike->arrival_time >= evo->now ) && (spike->arrival_time < evo->now + evo->dt)){
                // Excitatory
                if (spike->weight > 0.0){ this->state[1] += spike->weight;} 
                // Inhibitory
                else if (spike->weight < 0.0){ this->state[2] -= spike->weight;}
                // Spurious zero-weight
                else{
                    cout << "Warning: a zero-weighted spike was received" << endl;
                    cout << "\tweight is " << spike->weight<< endl; 
                }
                spike->processed = true;

                // Removes the spike from the incoming spikes
                this->incoming_spikes.pop();
            } else {
                // If a spike is not to process, neither the rest will be
                break;
            }
        }else{
            cout << "spike already processed" << endl;
        }
    }
}

void Neuron::evolve(EvolutionContext * evo){
    // Gather spikes
    this-> handle_incoming_spikes(evo);

    // Evolve
    this->evolve_state(evo);
    
    // Spike generation
    if ((this -> state[0]) > this->E_thr){ this -> spike(evo);}
}

void Neuron::spike(EvolutionContext * evo){
    for (auto synapse : this->efferent_synapses){ (*synapse).fire(evo); }

    this -> last_spike_time = evo -> now;
    this -> state[0] = this->E_rest;

    ((this->population)->n_spikes_last_step) ++;
}

// *************************** More detailed models ************************ //

void aqif_neuron::evolve_state(EvolutionContext * evo){
    if ( (evo->now) > (this->last_spike_time) + (this->tau_refrac) ){
        // Membrane decay
        this->state[0] -= ( this->state[0] - this->E_rest) * evo->dt / this->tau_m;
        // Synaptic currents
        this -> state [0] -= evo -> dt * (this -> state [1])*( this -> state[0] - this -> E_exc);
        this -> state [0] -= evo -> dt * (this -> state [2])*( this -> state[0] - this -> E_inh);
    }
    // Conductances
    this->state[1] -= (this->state[1]) * (evo->dt) / (this->tau_e);
    this->state[2] -= (this->state[2]) * (evo->dt) / (this->tau_i);
}
