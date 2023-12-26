#pragma once

class HierarchicalID{
    // Object to build nested identification systems
    public:
        HierarchicalID * parent;
        unsigned int local_id;
        unsigned int current_max_subclass_id;

        HierarchicalID():parent(NULL),local_id(-1),current_max_subclass_id(0){}
        HierarchicalID(HierarchicalID * parent): parent(parent){
            //  The inner container constructor
            this->local_id = this->parent->current_max_subclass_id;
            this->parent->current_max_subclass_id ++;
            this->current_max_subclass_id = 0;
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