#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <variant>

#define WEIGHT_EPS 0.00001

// The menu

class HierarchicalID;
class EvolutionContext;

class Neuron;

class Projection;
class Population;
class SpikingNetwork;

class PopulationMonitor;
class PopCurrentInjector;


class Projection{
    public:
        int start_dimension, end_dimension;
        double ** weights, **delays;

        Projection(double ** weights, double ** delays, int start_dimension, int end_dimension);
};

class Population{
    public:
        int n_neurons;
        std::vector<Neuron*> neurons;
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
        std::vector<Population*> populations;
        HierarchicalID * id;

        // Monitors (output)
        std::vector<PopulationMonitor*> population_monitors;

        // Injectors (input)
        std::vector<PopCurrentInjector*> injectors;

        SpikingNetwork();

        void add_monitor(PopulationMonitor * monitor){
            std::cout<< "Adding monitor" << std::endl;
            this->population_monitors.push_back(monitor);
            };
        void add_injector(PopCurrentInjector * injector){this->injectors.push_back(injector);}

        void evolve(EvolutionContext * evo);
        void run(EvolutionContext * evo, double time);
};