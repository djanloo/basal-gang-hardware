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
    int Na = 5000;
    int Nb = 5000;

    SpikingNetwork sn = SpikingNetwork();
    Population a = Population(Na, neuron_type::aqif, &sn);
    Population b = Population(Nb, neuron_type::aqif, &sn);

    double ** weights, **delays;

    weights = get_rand_proj_mat(Na, Nb);
    delays = get_rand_proj_mat(Na,Nb);

    for (int i = 0; i < Na; i ++){
        for (int j=0; j < Nb; j++){
            if (weights[i][j] < 0.8){
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
    for (int i=0; i < n_steps; i++){
        sn.evolve(&evo);
        cout << "spikes  a: " << a.n_spikes_last_step << endl;
        cout << "spikes  b: " << b.n_spikes_last_step << endl;
    }
    auto end = chrono::high_resolution_clock::now();

    cout << "simulation took " << (chrono::duration_cast<chrono::seconds>(end -start)).count() << " ms";
    cout << "\t(" << ((double)(chrono::duration_cast<chrono::seconds>(end -start)).count())/n_steps << " ms/step)" << endl;
}

