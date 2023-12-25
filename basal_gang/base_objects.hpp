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
            this -> parent = _parent;

            std::cout << "giving child the current max id: "<< _parent -> current_max_subclass_id << std::endl;
            this -> local_id = _parent -> current_max_subclass_id;

            std::cout << "increasing max subclass form " << _parent -> current_max_subclass_id << " to " << (_parent -> current_max_subclass_id) + 1 << " of ID at" << _parent << std::endl;
            (_parent -> current_max_subclass_id) ++;
            this -> current_max_subclass_id = 0;
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