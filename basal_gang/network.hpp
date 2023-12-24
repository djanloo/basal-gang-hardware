#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "base_objects.hpp"
#include "neurons.hpp"

class Population{
    public:
        vector<Neuron*> neurons;
        int n_neurons;
        Population(int n_neurons, neuron_type nt);
        void project(Projection * projection, Population * child_pop);
        void evolve(EvolutionContext * evo);
};

class Projection{
    public:
        int start_dimension, end_dimension;
        double ** weights, **delays;

        Projection(double ** weights, double ** delays, int start_dimension, int end_dimension);
};

class SpikingNetwork{
    public:
        vector<Population*> populations;
        double dt;

        SpikingNetwork(vector<Population*> _populations){
            this -> populations = _populations;
        }

        void evolve(EvolutionContext * evo);
        void run(EvolutionContext * evo, time);
};