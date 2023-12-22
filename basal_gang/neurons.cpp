#include <iostream>
#include <vector>
#include <list>
#include "neurons.hpp"

using namespace std;


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
    nt = aqif;
}

void aqif_neuron::evolve(){
    this -> state [0] += 0.1;
}

// Populations stuff

Population::Population(int n_neurons, neuron_type nt){

    Neuron * new_neuron;

    for ( int i = 0; i < n_neurons; i++){
        switch(nt){
        case dummy  : 
            neurons.push_back(new Neuron());
            break;

        case aqif:
            neurons.push_back(new aqif_neuron());
            break;
    };
    }
}

void Population::project(Population * child_pop ){
    
}

int main(){
    Population a = Population(100, (neuron_type) aqif);

    for (auto neuron : a.neurons){
        cout << neuron -> state[0] << endl;
        cout << "neuron type:" << neuron -> nt << endl;
    }
}
