#pragma once
#include <iostream>
#include <variant>

#include "base_objects.hpp"
#include "neurons.hpp"
#include "network.hpp"

template <typename obj>

/*Class to monitor other stuff. Pretty generic, just build it on a pointer */
class Monitor{
    public:
        using monitor_type = decltype(std::declval<obj>()->monitor());
        Monitor(obj  object){
            this->monitored_object = object;
        };
        void gather(){
            this->history.push_back(this->monitored_object->monitor());
        }
        vector<variant<monitor_type>> get_history(){
            return this->history;
        }
    private:
        obj monitored_object;
        vector<variant<monitor_type>> history;
};