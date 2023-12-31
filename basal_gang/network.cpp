#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "include/base_objects.hpp"
#include "include/neurons.hpp"
#include "include/network.hpp"
#include "include/devices.hpp"


Population::Population(int n_neurons, neuron_type nt, SpikingNetwork * spiking_network){
    this -> n_neurons = n_neurons;
    this -> n_spikes_last_step = 0;
    this -> id = new HierarchicalID(spiking_network->id);
    
    auto start = chrono::high_resolution_clock::now();

    for ( int i = 0; i < n_neurons; i++){
        // This can be avoided, probably using <variant>
        switch(nt){
        case neuron_type::dummy: new Neuron(this); break;       // remember not to push_back here
        case neuron_type::aqif: new aqif_neuron(this); break;   // calling the constructor is enough
        };
    }

    auto end = chrono::high_resolution_clock::now();
    cout << "Building population "<< this->id->get_id() << " took " << (chrono::duration_cast<chrono::milliseconds>(end -start)).count() << " ms    (";
    cout << ((double)(chrono::duration_cast<chrono::microseconds>(end-start)).count())/n_neurons << " us/neur)" << endl;
    
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
    cout << "pop " << this->id->get_id() << ") average synaptic queue is long " << avg_synaptic_queue_size << endl;

    this->n_spikes_last_step = 0;
    auto start = chrono::high_resolution_clock::now();
    for (auto neuron : this -> neurons){
        neuron -> evolve(evo);
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "pop " <<this->id->get_id() << ") evolving took " << ((double)(chrono::duration_cast<chrono::milliseconds>(end-start)).count()) << " ms (";
    cout << ((double)(chrono::duration_cast<chrono::microseconds>(end-start)).count())/this->n_neurons;
    cout << " us/neur )" << endl;
}

int Population::monitor(){
    return this-> n_spikes_last_step;
}

void  SpikingNetwork::evolve(EvolutionContext * evo){
    for (auto population : this -> populations){
        population -> evolve(evo);
    }
    evo -> do_step();
}

SpikingNetwork::SpikingNetwork(){
    this->id = new HierarchicalID();
}

void SpikingNetwork::run(EvolutionContext * evo, double time){  

    /**
     * Gets the values form monitors. 
     * This may be a little too formal, but 
     *      - cycles on auto references of variant (auto&)
     *      - avoid modification of reference (const)
     * 
     * Also maybe too pythonic. I just have to quit heterogeneous iterables.
     * 
     * TODO: check timing. This is a lot of overhead I see here.
     * 
     */

    auto start = chrono::high_resolution_clock::now();
    int n_steps = 0;

    auto gather_time = chrono::duration_cast<chrono::microseconds>(start-start).count();
    auto inject_time = chrono::duration_cast<chrono::microseconds>(start-start).count();

    // Evolve
    while (evo -> now < time){
        cout << "------ Time: " << evo-> now << "---------" << endl;
        auto start_gather = chrono::high_resolution_clock::now();
        for (const auto& population_monitor : this->population_monitors){
            population_monitor->gather();
        }
        auto end_gather = chrono::high_resolution_clock::now();
        gather_time += chrono::duration_cast<chrono::microseconds>(end_gather-start_gather).count();

        auto start_inject = chrono::high_resolution_clock::now();
        for (auto injector : this->injectors){
            injector->inject(evo);
        }
        auto end_inject = chrono::high_resolution_clock::now();
        inject_time += chrono::duration_cast<chrono::microseconds>(end_inject-start_inject).count();

        for (auto population : this -> populations){
            population -> evolve(evo);
        }
        evo -> do_step();
        n_steps++;
    }

    auto end = chrono::high_resolution_clock::now();
    cout << "simulation took " << (chrono::duration_cast<chrono::seconds>(end -start)).count() << " s";
    cout << "\t(" << ((double)(chrono::duration_cast<chrono::seconds>(end -start)).count())/n_steps << " s/step)" << endl;

    cout << "\tGathering time avg: " << static_cast<double>(gather_time)/n_steps << " us/step" << endl;
    cout << "\tInject time avg: " << static_cast<double>(inject_time)/n_steps << " us/step" << endl;


}