#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "base_objects.hpp"
#include "neurons.hpp"

class Population;
class SpikingNetwork;
class Projection;

class Population{
    public:
        int n_neurons;
        vector<Neuron*> neurons;
        HierarchicalID * id;

        Population(int n_neurons, neuron_type nt, SpikingNetwork * spiking_network);
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
        HierarchicalID * id;

        SpikingNetwork(){
            this->id = new HierarchicalID();
        }

        void evolve(EvolutionContext * evo);
        void run(EvolutionContext * evo, double time);
};