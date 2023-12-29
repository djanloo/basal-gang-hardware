#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <variant>

#include "base_objects.hpp"
#include "neurons.hpp"

#define WEIGHT_EPS 0.00001

// The menu
template <class obj, typename result> class Monitor;
template <typename var> class Injector;
class Projection;
class Population;
class SpikingNetwork;



class Projection{
    public:
        int start_dimension, end_dimension;
        double ** weights, **delays;

        Projection(double ** weights, double ** delays, int start_dimension, int end_dimension);
};

class Population{
    public:
        int n_neurons;
        vector<Neuron*> neurons;
        HierarchicalID * id;

        // Biophysical attributes
        int n_spikes_last_step;

        Population(int n_neurons, neuron_type nt, SpikingNetwork * spiking_network);
        void project(Projection * projection, Population * child_pop);
        void evolve(EvolutionContext * evo);
        int monitor();
};

class SpikingNetwork{
    public:
        vector<Population*> populations;
        HierarchicalID * id;


        // Monitors (output)
        vector <variant < Monitor<Population, int>*, Monitor <Neuron, neuron_state > * >> monitors;

        // Injectors (input)
        vector<Injector<double>*> injectors;
        
        SpikingNetwork(){
            this->id = new HierarchicalID();
        }

        template <class obj, typename res>
        void add_monitor(Monitor<obj, res> * monitor);

        void evolve(EvolutionContext * evo);
        void run(EvolutionContext * evo, double time);
};