#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "include/base_objects.hpp"
#include "include/neurons.hpp"
#include "include/devices.hpp"
#include "include/network.hpp"

Population::Population(int n_neurons, neuron_type nt, SpikingNetwork * spiking_network){
    this -> n_neurons = n_neurons;
    this -> n_spikes_last_step = 0;
    this -> id = new HierarchicalID(spiking_network->id);
    
    auto start = chrono::high_resolution_clock::now();

    for ( int i = 0; i < n_neurons; i++){
        // The big switch, in Python this would be easier.
        // It's not particulary efficient but it has to be done
        // just once so nvm
        switch(nt){
        case neuron_type::dummy: new Neuron(this); break;       // remember not to push_back here
        case neuron_type::aqif: new aqif_neuron(this); break;   // calling the constructor is enough
        };
    }

    auto end = chrono::high_resolution_clock::now();
    cout << "Building population "<< this->id->local_id << " took " << (chrono::duration_cast<chrono::milliseconds>(end -start)).count() << " ms    (";
    cout << ((double)(chrono::duration_cast<chrono::microseconds>(end -start)).count())/n_neurons << " us/neur)" << endl;
    
    // Adds itself to the spiking network populations
    (spiking_network->populations).push_back(this);
    }

Projection::Projection(double ** _weights, double ** _delays, int _start_dimension, int _end_dimension){
    this -> weights = _weights;
    this -> delays = _delays;
    this -> start_dimension = _start_dimension;
    this -> end_dimension = _end_dimension;

    int n_links = 0;
    for (int i = 0; i < _start_dimension; i++){
        for (int j =0 ; j< _end_dimension; j++){
            if (weights[i][j] != 0.0){ //Mhh, dangerous
                n_links ++;
            }
        }
    }

    //  cout << "Projection: printing weights:" <<endl;

    // for (int i = 0; i < _start_dimension; i ++){
    //     for (int j=0; j < _end_dimension; j++){
    //         cout << weights[i][j] << "\t";
    //     }
    //     cout << endl;
    // }

    // cout << "Projection: printing delays:" <<endl;

    // for (int i = 0; i < _start_dimension; i ++){
    //     for (int j=0; j < _end_dimension; j++){
    //         cout << delays[i][j] << "\t";
    //     }
    //     cout << endl;
    // }
    cout << "projection has density "<< ((float)n_links)/_start_dimension/_end_dimension * 100 << "%" << endl;
}

void Population::project(Projection * projection, Population * efferent_population){
    int connections = 0;
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < projection -> start_dimension; i++){
        for (int j = 0; j < projection -> end_dimension; j++){
            if (abs((projection -> weights)[i][j]) > WEIGHT_EPS){
                connections ++;
                (this -> neurons)[i] -> connect(efferent_population -> neurons[j], (projection -> weights)[i][j], (projection -> delays)[i][j]);
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "performing " << connections << " connections took " << (chrono::duration_cast<chrono::milliseconds>(end -start)).count() << " ms   (";
    cout << ((double)(chrono::duration_cast<chrono::microseconds>(end -start)).count())/connections << " us/link)" << endl;
}

void Population::evolve(EvolutionContext * evo){

    double avg_synaptic_queue_size = 0;
    for (auto neuron : this->neurons){
        avg_synaptic_queue_size += neuron -> incoming_spikes.size();
    }
    avg_synaptic_queue_size /= this->n_neurons;
    cout << "pop " << this->id->local_id << ") average synaptic queue is long " << avg_synaptic_queue_size << endl;

    this->n_spikes_last_step = 0;
    auto start = chrono::high_resolution_clock::now();
    for (auto neuron : this -> neurons){
        neuron -> evolve(evo);
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "pop " <<this->id->local_id << ") evolving took " << ((double)(chrono::duration_cast<chrono::milliseconds>(end-start)).count()) << " ms (";
    cout << ((double)(chrono::duration_cast<chrono::microseconds>(end-start)).count())/this->n_neurons;
    cout << " us/neur )" << endl;
}

int Population::monitor(){
    return this-> n_spikes_last_step;
}

void  SpikingNetwork::evolve(EvolutionContext * evo){
    for (auto population : this -> populations){
        // cout << "spikingnetwork called for evolution of population" << population->id->local_id << endl;
        population -> evolve(evo);
    }
    evo -> do_step();
}
template<class obj, typename res> void SpikingNetwork::add_monitor(Monitor<obj, res> * monitor){
    this->monitors.push_back(monitor);
}

void SpikingNetwork::run(EvolutionContext *evo, double time){
    
    /**Gets the values form monitors. 
     * This may be a little too formal, but 
     *      - cycles on auto references of variant (auto&)
     *      - avoid modification of reference (const)
     * 
     * Also maybe too pythonic. I just have to quit heterogeneous iterables.
     * 
     * TODO: check timing. This is a lot of overhead I see here.
     * 
     */
    for (const auto& monitor_variant : this->monitors){
        if (holds_alternative<Monitor<Population, int>*>(monitor_variant)) {
            auto& population_monitor = get<Monitor<Population,int>*>(monitor_variant);
            population_monitor->gather();
        } else if (holds_alternative<Monitor<Neuron, neuron_state>*>(monitor_variant)) {
            auto& neuron_monitor = get<Monitor<Neuron, neuron_state>*>(monitor_variant);
            neuron_monitor->gather();
        }
    }

    auto start = chrono::high_resolution_clock::now();
    int n_steps = 0;
    // Evolve
    while (evo -> now < time){
        for (auto population : this -> populations){
        population -> evolve(evo);
        }
        evo -> do_step();
        n_steps++;
    }

    auto end = chrono::high_resolution_clock::now();
    cout << "simulation took " << (chrono::duration_cast<chrono::seconds>(end -start)).count() << " s";
    cout << "\t(" << ((double)(chrono::duration_cast<chrono::seconds>(end -start)).count())/n_steps << " s/step)" << endl;
}