#include <iostream>

#include "base_objects.hpp"
#include "neurons.hpp"
#include "network.hpp"


double ** get_rand_proj_mat(int N, int M){
    double** matrix = new double*[N];
    for (int i = 0; i < N; ++i) {
        matrix[i] = new double[M];
    }

    for (int i=0;i<N;i++){
        for (int j=0; j< M; j++){
            matrix[i][j] = 2*(((double) rand())/RAND_MAX - 0.5);
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
    int Na = 500;
    int Nb = 500;
    Population a = Population(Na, neuron_type::aqif);
    Population b = Population(Nb, neuron_type::aqif);

    double ** weights, **delays;

    weights = get_rand_proj_mat(Na, Nb);
    delays = get_rand_proj_mat(Na,Nb);

    for (int i = 0; i < Na; i ++){
        for (int j=0; j < Nb; j++){
            if (weights[i][j] < 0.1){
                weights[i][j] = 0.0;
                delays[i][j] = 0.0;
            }
        }
    }

    Projection * projection = new Projection(weights, delays, Na, Nb);

    a.project(projection, &b);

    delete projection;
    free_proj_mat(weights, Na);
    free_proj_mat(delays, Nb);

    vector<Population*> populations = vector<Population*> {&a, &b};

    SpikingNetwork net = SpikingNetwork(populations);
    EvolutionContext evo = EvolutionContext(0.1);
    for (int i=0; i < 100; i++){
        net.evolve(&evo);
        cout << "time :" << evo.now << endl;
        cout << "N0-V: " <<  a.neurons[0]->state[0] << endl;
        cout << "N0-ge: " << a.neurons[0]->state[1] << endl;
        cout << "N0-gi: " << a.neurons[0]->state[2] << endl;
    }
}

