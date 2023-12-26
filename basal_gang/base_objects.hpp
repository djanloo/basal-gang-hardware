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
            this -> local_id = _parent -> current_max_subclass_id;
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