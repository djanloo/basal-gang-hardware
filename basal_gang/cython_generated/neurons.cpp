#include <iostream>
#include <vector>
#include <list>
#include <map>

#include "neurons.hpp"

using namespace std;

dummy_obj::dummy_obj(){
    bro = 10;
};

Neuron::Neuron(){
    state = vector<double> { ((float)rand())/RAND_MAX, ((float)rand())/RAND_MAX};
};

void Neuron::connect(Neuron * neuron_ptr){
    childs.push_back(neuron_ptr);
}

void Neuron::evolve(){
    state[0] += 1.5;
};

void Neuron::spike(){
    for (auto child : childs){
        child -> state [0] += 3.0;
    }
}

aqif_neuron::aqif_neuron(){
    nt = neuron_type::aqif;
}

void aqif_neuron::evolve(){
    this -> state [0] += 0.1;
}

// Populations stuff
Population::Population(int n_neurons, neuron_type nt){
    cout << "making a population of " << n_neurons << "neurons" << endl;
    this -> n_neurons = n_neurons;

    for ( int i = 0; i < n_neurons; i++){
        switch(nt){
        case neuron_type::dummy: 
            neurons.push_back(new Neuron());
            break;

        case neuron_type::aqif:
            neurons.push_back(new aqif_neuron());
            break;
    };
    }
}

Projection::Projection(double ** memoryview, int start_dimension, int end_dimension){
    cout << "requested projection between " << start_dimension << " objects and " << end_dimension << " objects"<< endl;
    for (int i = 0; i < start_dimension;i++){
        for (int j = 0; j < end_dimension; j++){
            cout << memoryview[i][j] << " ";
        }
        cout << endl;
    }
}

void Population::project(Population * child_pop){
    
}

int main(){
    Population a = Population(100, neuron_type::aqif);

    for (auto neuron : a.neurons){
        cout << neuron -> state[0] << endl;
        // cout << "neuron type:" << neuron -> nt << endl;
    }

    cout << "Pop has number: " << a.n_neurons << endl;
}
