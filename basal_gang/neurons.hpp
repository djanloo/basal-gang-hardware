#pragma once
#include <iostream>
#include <vector>
#include <list>

using namespace std;

enum class neuron_type : unsigned int {dummy, aqif};

class EvolutionContext;
class Spike;
class Axon;
class Neuron;
class Population;
class Projection;


class EvolutionContext{
    public:
        double now; // time in millis
        double dt; // timestep in millis

        EvolutionContext(double _dt){
            this -> dt = _dt;
            this -> now = 0.0;
        }
        void do_step();
};

class Spike{
    public:
        double weight, arrival_time;
        // Remember to deallocate once you're done with me !
        Spike(double _weight, double _arrival_time){
            this -> weight = _weight;
            this -> arrival_time = _arrival_time;
        }
};

class Axon{
    public:
        Neuron * presynaptic;
        Neuron * postsynaptic;
        double weight, delay;

        Axon(Neuron * _presynaptic, Neuron * _postsynaptic, double _weight, double _delay);
        void fire(EvolutionContext * evo);
};

class Neuron{
    public:
        vector<double> state;
        int state_dimension;
        neuron_type nt = neuron_type::dummy;
        int index;

        vector<Axon*> efferent_axons;
        vector<Spike*> incoming_exc_spikes, incoming_inh_spikes;

        Neuron(int _index);
        
        void connect(Neuron * neuron, double weight, double delay);
        void spike(EvolutionContext * evo);
        virtual void evolve(EvolutionContext * evo);
};


class aqif_neuron : public Neuron {
    public:
        aqif_neuron(int _index);
        void evolve(EvolutionContext * evo);
};

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
        void evolve(EvolutionContext * evo);
        SpikingNetwork(vector<Population*> _populations){
            this -> populations = _populations;
        }
};