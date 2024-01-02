#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <chrono>

#include "include/base_objects.hpp"
#include "include/devices.hpp"
#include "include/neurons.hpp"
#include "include/network.hpp"

#define MAX_POTENTIAL_INCREMENT 10 // mV

void Synapse::fire(EvolutionContext * evo){
    this->postsynaptic->incoming_spikes.emplace(this->weight, evo->now + this->delay);
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

    while (!(this->incoming_spikes.empty())){
        
        auto spike = this->incoming_spikes.top();

        if ((spike.arrival_time < evo->now)&(!spike.processed)){cout << "ERROR: spike missed" << endl;} 

        if (!(spike.processed)){

            if ((spike.arrival_time >= evo->now ) && (spike.arrival_time < evo->now + evo->dt)){
                // Excitatory
                if (spike.weight > 0.0){ this->state[1] += spike.weight;} 
                // Inhibitory
                else if (spike.weight < 0.0){ this->state[2] -= spike.weight;}
                // Spurious zero-weight
                else{
                    cout << "Warning: a zero-weighted spike was received" << endl;
                    cout << "\tweight is " << spike.weight<< endl; 
                }
                spike.processed = true;

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
    double V_m = this->state[0];

    if ( (evo->now) > (this->last_spike_time) + (this->tau_refrac) ){
        // Membrane decay
        this->state[0] -= ( V_m - this->E_rest) * evo->dt / this->tau_m;
        // Synaptic currents
        this -> state [0] -= evo -> dt * (this -> state [1])*(  V_m - this -> E_exc);
        this -> state [0] -= evo -> dt * (this -> state [2])*(  V_m - this -> E_inh);
    }
    // Conductances
    this->state[1] -= (this->state[1]) * (evo->dt) / (this->tau_e);
    this->state[2] -= (this->state[2]) * (evo->dt) / (this->tau_i);
}

izhikevich_neuron::izhikevich_neuron(Population * population): Neuron(population){
    this->nt = neuron_type::izhikevich;

    this -> state = vector<double> { 
                                    this -> E_rest + ((double)rand())/RAND_MAX, // V
                                    0.0, // g_syn_exc
                                    0.0, // g_syn_inh
                                    0.0 // u
                                    };
    
    this-> a = 0.02;
    this-> b = 0.2;
    this-> c = -65;
    this-> d = 8;
}

void izhikevich_neuron::evolve_state(EvolutionContext * evo){
    double V_m = this->state[0];
    double u = this->state[3];

    this->state[0] += evo->dt * ( 0.04*V_m*V_m + 5*V_m + 140 - u); // V
    this->state[3] += evo->dt * this->a * ( this->b * V_m - u);  // u

    // Synaptic currents
    this -> state [0] -= evo->dt * (this->state [1])*(  V_m - this->E_exc);
    this -> state [0] -= evo->dt * (this->state [2])*(  V_m - this->E_inh);

    // Conductances
    this->state[1] -= (this->state[1]) * (evo->dt) / (this->tau_e);
    this->state[2] -= (this->state[2]) * (evo->dt) / (this->tau_i);

    if (abs(this->state[0] - V_m)> MAX_POTENTIAL_INCREMENT){std::cout << "WARNING: membrane potential increment over maximum"<< std::endl;}

}

void izhikevich_neuron::spike(EvolutionContext * evo){ 

    for (auto synapse : this->efferent_synapses){ (*synapse).fire(evo); }

    this->state[0]  = this->E_rest;
    this->state[3] += this->d;

    this->last_spike_time = evo->now;
    ((this->population)->n_spikes_last_step) ++;

}