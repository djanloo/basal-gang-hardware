#pragma once

class HierarchicalID{
    // Object to build nested identification systems
    public:
        HierarchicalID * parent;
        unsigned int local_id;
        unsigned int current_max_subclass_id;

        HierarchicalID(){
            // The outer constructor
            this -> parent = NULL;
            this -> local_id = -1;
            this -> current_max_subclass_id = 0;
        }
        HierarchicalID(HierarchicalID * _parent){
            //  The inner container constructor
            this -> current_max_subclass_id = 0;
            this -> parent = _parent;
            this -> local_id = _parent -> current_max_subclass_id;
            (_parent -> current_max_subclass_id) += 1;
        }

};

class EvolutionContext{
    public:
        double now; // time in millis
        double dt;  // timestep in millis

        EvolutionContext(double _dt){
            this -> dt = _dt;
            this -> now = 0.0;
        }
        void do_step(){
            this -> now += this -> dt;
        }
};

class Spike{
    public:
        double weight, arrival_time;
        bool processed;
        // Remember to deallocate once you're done with me !
        Spike(double _weight, double _arrival_time){
            this -> weight = _weight;
            this -> arrival_time = _arrival_time;
            this -> processed = false;
        }
};