#pragma once
#include <iostream>
#include <variant>

#include "base_objects.hpp"
#include "neurons.hpp"
#include "network.hpp"


/**
 * A class to watch what the others are doing.
 * Type of object and variable are templated,
 * be sure your monitored object has a 'monitor()' method.
*/
template <class obj, typename result>
class Monitor{
    public:
        // Deduces the type of the return variable of monitor()

        Monitor(obj * object){this->monitored_object = object;};
        void gather(){
            this->history.push_back(this->monitored_object->monitor());
        }
        vector<result> get_history(){
            return this->history;
        }
    private:
        obj * monitored_object;
        vector<result> history;
};

/**
 * The input for neurons.
*/
template <typename var>
class Injector {
public:
    Injector( var * _variable_ptr, double rate, double t_max)
        : variable_ptr(_variable_ptr), rate(rate), t_max(t_max) {}

    void inject(EvolutionContext *evo) {
        if (evo->now < t_max && variable_ptr) {
            *variable_ptr += rate * evo->dt;
        }
    }

private:
    var * variable_ptr;
    double rate, t_max;
};
