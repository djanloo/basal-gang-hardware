#pragma once

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