#include "timing.hpp"
#include "logging.hpp"
#include "../global.hpp"

using namespace std;

Timing::Timing() {
    _prev_notify = chrono::high_resolution_clock::now();
}

void Timing::set_name(std::string name) {
    _name = name;
}

void Timing::set_name(char* name) {
    _name = string(name);
}

void Timing::op_done() {
    chrono::high_resolution_clock::time_point now = chrono::high_resolution_clock::now();
    double time_passed = chrono::duration_cast<chrono::duration<double>>(now - _prev_notify).count();
    _ops_passed++;

    if(time_passed >= TIMING_STATISTIC_INTERVAL) {
        _prev_notify = now;
        cmessage << color(FG_BLUE) << _name << color(FG_DEFAULT) << ": " << color(FG_GREEN) << (1.0 * _ops_passed / time_passed) << color(FG_DEFAULT) << " op/s (" << color(FG_GREEN) << _ops_passed << color(FG_DEFAULT) << " ops in " << color(FG_GREEN) << time_passed << color(FG_DEFAULT) << "s)" << endlog;
        _ops_passed = 0;
    }
}