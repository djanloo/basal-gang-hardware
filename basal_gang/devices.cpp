#include "include/neurons.hpp"
#include "include/network.hpp"
#include "include/base_objects.hpp"
#include "include/devices.hpp"

void PopulationMonitor::gather(){
            this->history.push_back(this->monitored_pop->monitor());
            }

void PopCurrentInjector::inject(EvolutionContext * evo){
    if (evo->now < this->t_max){
        for (auto neuron : this-> pop->neurons){
            neuron->state[0] += (this->I) * (evo->dt);
        }
    }
}
