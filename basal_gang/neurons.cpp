#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <chrono>

#include "neurons.hpp"

using namespace std;


void Axon::fire(EvolutionContext * evo){
    // EFFICIENCY ALERT: this takes too much time, I know
    ((this -> postsynaptic) -> incoming_spikes).push_back(new Spike(this->weight, (evo -> now) + this->delay));
    return;
}

Neuron::Neuron(int _index){
    this -> state = vector<double> { ((double)rand())/RAND_MAX - 1.0, 0.0, 0.0};
    this -> index = _index;

    // TODO: too much redundancy in parameters. 
    // If parameter is the same for the population
    // use a this -> population -> value
    this->E_exc = 0.0;      // mV
    this->E_inh = -80.0;    // mV
    this->E_rest = -60.0;   // mV
    this->E_thr = -50.0;     // mV
    
    this->tau_refrac = 10;  // ms
    this->tau_i = 10;
    this->tau_e = 5;
    this->tau_m = 15;

    this-> last_spike_time = -1;
};

void Neuron::connect(Neuron * neuron, double weight, double delay){
    (this -> efferent_axons).push_back(new Axon(this, neuron, weight, delay));
}

void Neuron::evolve(EvolutionContext * evo){
    state[0] += 1.5; // A nice dummy nonsense, impossible to not see this :)
};

void Neuron::spike(EvolutionContext * evo){
    // cout << "neuron " << this-> index << " spiked. Now its " << evo->now << " and last spike was " << this->last_spike_time << endl;
    for (auto axon : this->efferent_axons){
        (*axon).fire(evo);
    }
    this -> last_spike_time = evo -> now;
    this -> state[0] = this->E_rest;
}

aqif_neuron::aqif_neuron(int _index):Neuron( _index){
    this -> nt = neuron_type::aqif;
}

void aqif_neuron::evolve(EvolutionContext * evo){

    // Spike processing
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
    if (evo->now > this->last_spike_time + this->tau_refrac ){
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

Population::Population(int n_neurons, neuron_type nt){
    this -> n_neurons = n_neurons;

    for ( int i = 0; i < n_neurons; i++){
        switch(nt){
        case neuron_type::dummy: 
            neurons.push_back(new Neuron(i));
            break;

        case neuron_type::aqif:
            neurons.push_back(new aqif_neuron(i));
            break;
    };
    }
}

Projection::Projection(double ** _weights, double ** _delays, int _start_dimension, int _end_dimension){
    this -> weights = _weights;
    this -> delays = _delays;
    this -> start_dimension = _start_dimension;
    this -> end_dimension = _end_dimension;
}

void Population::project(Projection * projection, Population * efferent_population){
    int connections = 0;
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < projection -> start_dimension; i++){
        for (int j = 0; j < projection -> end_dimension; j++){
            if ((projection -> weights)[i][j] != 0.0){
                connections ++;
                (this -> neurons)[i] -> connect(efferent_population -> neurons[j], (projection -> weights)[i][j], (projection -> delays)[i][j]);
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "performed " << connections << " connections" << endl;
    cout << "connection took " << (chrono::duration_cast<chrono::milliseconds>(end -start)).count() << " ms" << endl;
}

void Population::evolve(EvolutionContext * evo){
    for (auto neuron : this -> neurons){
        neuron -> evolve(evo);
    }
}

void  SpikingNetwork::evolve(EvolutionContext * evo){
    for (auto population : this -> populations){
        population -> evolve(evo);
    }
    evo -> do_step();
}

void EvolutionContext::do_step(){
    this -> now += this -> dt;
}

double ** get_rand_proj_mat(int N, int M){
    double** matrix = new double*[N];
    for (int i = 0; i < N; ++i) {
        matrix[i] = new double[M];
    }

    for (int i=0;i<N;i++){
        for (int j=0; j< M; j++){
            matrix[i][j] = 2*(((double) rand())/RAND_MAX - 0.5);
        }
    }

    return matrix;
}

void free_proj_mat(double** matrix, int N) {
    for (int i = 0; i < N; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}


int main(){
    int Na = 500;
    int Nb = 500;
    Population a = Population(Na, neuron_type::aqif);
    Population b = Population(Nb, neuron_type::aqif);

    double ** weights, **delays;

    weights = get_rand_proj_mat(Na, Nb);
    delays = get_rand_proj_mat(Na,Nb);

    for (int i = 0; i < Na; i ++){
        for (int j=0; j < Nb; j++){
            if (weights[i][j] < 0.1){
                weights[i][j] = 0.0;
                delays[i][j] = 0.0;
            }
        }
    }

    Projection * projection = new Projection(weights, delays, Na, Nb);

    a.project(projection, &b);

    delete projection;
    free_proj_mat(weights, Na);
    free_proj_mat(delays, Nb);

    vector<Population*> populations = vector<Population*> {&a, &b};

    SpikingNetwork net = SpikingNetwork(populations);
    EvolutionContext evo = EvolutionContext(0.1);
    for (int i=0; i < 100; i++){
        net.evolve(&evo);
        cout << "time :" << evo.now << endl;
        cout << "N0-V: " <<  a.neurons[0]->state[0] << endl;
        cout << "N0-ge: " << a.neurons[0]->state[1] << endl;
        cout << "N0-gi: " << a.neurons[0]->state[2] << endl;
    }
}
