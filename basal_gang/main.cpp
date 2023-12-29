#include <iostream>
#include <fstream>

#include "include/base_objects.hpp"
#include "include/neurons.hpp"
#include "include/network.hpp"
#include "include/devices.hpp"

// ulimit -c unlimited
// sudo sysctl -w kernel.core_pattern=/tmp/core-%e.%p.%h.%t

double rand_01(){
    return ((double)rand())/RAND_MAX;
}

double ** get_rand_proj_mat(int N, int M, double min, double max){
    double** matrix = new double*[N];
    for (int i = 0; i < N; ++i) {
        matrix[i] = new double[M];
    }

    for (int i=0;i<N;i++){
        for (int j=0; j< M; j++){
            matrix[i][j] = min + (max-min)*rand_01();
        }
    }
    return matrix;
}

void free_proj_mat(double** matrix, int N) {
    for (int i = 0; i < N; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

int main(){
    int Na = 1000;
    int Nb = 1000;

    SpikingNetwork sn = SpikingNetwork();
    Population a = Population(Na, neuron_type::aqif, &sn);
    Population b = Population(Nb, neuron_type::aqif, &sn);

    cout << "size of neuron is " << sizeof(*(a.neurons[0])) << " bytes" <<endl ;
    cout << "size of population is " << sizeof(a) << " bytes" << endl;

    double ** weights, **delays;

    weights = get_rand_proj_mat(Na,Nb, -0.02,0.08);
    delays = get_rand_proj_mat(Na,Nb, 0.5, 1.0);

    for (int i = 0; i < Na; i ++){
        for (int j=0; j < Nb; j++){
            if (rand_01() > 0.1){
                weights[i][j] = 0.0;
                delays[i][j] = 0.0;
            }
            if (abs(weights[i][j]) < WEIGHT_EPS){
                weights[i][j] = 0.0;
                delays[i][j] = 0.0;
            }
        }
    }


    Projection * projection = new Projection(weights, delays, Na, Nb);

    a.project(projection, &b);
    b.project(projection, &a);

    delete projection;
    free_proj_mat(weights, Na);
    free_proj_mat(delays, Nb);


    EvolutionContext evo = EvolutionContext(0.1);
    
    auto start  = chrono::high_resolution_clock::now();
    int n_steps = 100;

    vector< Monitor<Neuron, vector<double> >*> monitors;
    vector<Injector<double>*> injectors;
    for (int i=0; i<3;i++){
        monitors.push_back(new Monitor<Neuron, vector<double>>(a.neurons[i]));
        injectors.push_back(new Injector(&(a.neurons[i]->state[1]), 0.5, 5));
    }

    for (int i=0; i < n_steps; i++){
        cout << "--------- time " << evo.now << "---------------"<<endl;
        sn.evolve(&evo);
        for (auto monitor : monitors){ monitor->gather();}
        for (auto inj:injectors){inj->inject(&evo);}

        cout << "monitor history size: " << monitors[0]->get_history().size() << endl; 
        cout << "spikes  a: " << a.n_spikes_last_step << endl;
        cout << "spikes  b: " << b.n_spikes_last_step << endl;

    }
    auto end = chrono::high_resolution_clock::now();

    vector<vector<double>> hist = monitors[0]->get_history();
    for (auto h : hist){cout << h[0] << " ";} cout << endl;

    cout << "simulation took " << (chrono::duration_cast<chrono::seconds>(end -start)).count() << " s";
    cout << "\t(" << ((double)(chrono::duration_cast<chrono::seconds>(end -start)).count())/n_steps << " s/step)" << endl;
}

