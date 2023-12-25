#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "base_objects.hpp"
#include "neurons.hpp"
#include "network.hpp"

Population::Population(int n_neurons, neuron_type nt, SpikingNetwork * spiking_network){
    this -> n_neurons = n_neurons;
    this -> id = new HierarchicalID(spiking_network->id);
    
    auto start = chrono::high_resolution_clock::now();

    for ( int i = 0; i < n_neurons; i++){
        // The big switch, in Python this would be easier.
        // It's not particulary efficient but it has to be done
        // just once so nvm
        switch(nt){
        case neuron_type::dummy: neurons.push_back(new Neuron(this)); break;
        case neuron_type::aqif: neurons.push_back(new aqif_neuron(this)); break;
        };
    }

    auto end = chrono::high_resolution_clock::now();
    cout << "Building population "<< this->id->local_id << " took " << (chrono::duration_cast<chrono::milliseconds>(end -start)).count() << " ms" << endl;
    
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

void SpikingNetwork::run(EvolutionContext *evo, double time){
    
    while (evo -> now < time){
        for (auto population : this -> populations){
        population -> evolve(evo);
        }
        evo -> do_step();
    }
}